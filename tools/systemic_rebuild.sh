#!/usr/bin/env bash
# Systemic rebuild: fix the Ghidra project, re-dump every function, convert with the V2 rules,
# rebuild and link. Nothing here runs by itself; start it by hand:
#
#   tools/systemic_rebuild.sh                 # everything (~35-50 min)
#   SHARDS=4 tools/systemic_rebuild.sh        # fewer parallel Ghidra dumps (RAM: ~3 GB each)
#   FROM=build tools/systemic_rebuild.sh      # skip project fix + dumps (they are done)
#
# Steps (each logged in build/systemic/logs/<step>.log, overall log build/systemic/rebuild.log):
#   1. prep      native/robust/systemic/*.json (verdicts from the ARM code; generated only if
#                missing: tools/systemic_prep.py)
#   2. project   backup logs/ghidra_full -> logs/ghidra_full.bak_systemic_<date> (once), then the
#                Ghidra scripts albw_fix_noreturn.py, albw_fix_bodies.py, albw_fix_returns.py
#                (idempotent; skipped when build/systemic/project.stamp matches the inputs)
#   3. dump      ghidra_dump.py (ALBW_DUMP_V2=1: float bit-casts marked, removed starts as
#                placeholders) -> build/ghidra_analyzed3, in $SHARDS parallel processes on
#                private copies of the project (Ghidra locks a project even read-only)
#   4. extra     albw_dump_extra.py (V2) -> build/ghidra_extra3 + its USA->EU map
#   5. build     tools/build_decomp.py with ALBW_CONVERT_V2=1 ALBW_GHIDRA_DIR=build/ghidra_analyzed3
#                (converted sources under build/decomp/ghidra_fallback/ghidra_analyzed3/, the
#                default build's files are left alone)
#   6. names     tools/resolve_names.py
#   7. ctors     tools/ctor_regen.py (generated constructors only; hand-written files kept)
#   8. link      tools/link.sh
#
# Rollback (back to the current build): python3 tools/build_decomp.py && tools/link.sh
#   (report.json then lists the default ghidra_fallback objects again); the project backup is
#   logs/ghidra_full.bak_systemic_<date> (and logs/ghidra_full.bak_20261114 from before).
set -euo pipefail
cd "$(dirname "$0")/.."
P=$PWD
D=$(cd "$P/../albw-decomp-ai" && pwd)
GH=${GHIDRA_HEADLESS:-<your Ghidra 12.0.2 install>/support/analyzeHeadless}
PY=${ALBW_PY:-$D/.venv/bin/python}
PROJ=$D/logs/ghidra_full
SY=$P/native/robust/systemic
OUT=$P/build/systemic
LOGS=$OUT/logs
DUMP=$P/build/ghidra_analyzed3
EXTRA=$P/build/ghidra_extra3
SHARDS=${SHARDS:-6}
FROM=${FROM:-prep}
mkdir -p "$LOGS"
exec > >(tee -a "$OUT/rebuild.log") 2>&1
exec 9>"$OUT/.lock"
flock -n 9 || { echo "another systemic_rebuild.sh is running"; exit 1; }

step() { echo "[$(date '+%F %T')] $*"; }
order=(prep project dump extra build names ctors link)
want() {  # run step $1 if it is at or after $FROM
    local s seen=0
    for s in "${order[@]}"; do [[ $s == "$FROM" ]] && seen=1; [[ $s == "$1" ]] && { ((seen)); return; }; done
    return 1
}

step "systemic rebuild: from=$FROM shards=$SHARDS"
if [[ -e "$PROJ/ALBW_EU_FULL.lock" ]]; then
    step "Ghidra project is open elsewhere ($PROJ/ALBW_EU_FULL.lock); close it first"; exit 1
fi

# ---------------------------------------------------------------- 1. prep
if want prep; then
    if [[ -s $SY/noreturn.json && -s $SY/returns.json && -s $SY/bodies.json && -s $SY/fparams.json ]]; then
        step "prep: using native/robust/systemic/*.json"
    else
        step "prep: tools/systemic_prep.py"
        "$PY" tools/systemic_prep.py > "$LOGS/prep.log" 2>&1
    fi
fi

# ---------------------------------------------------------------- 2. project
if want project; then
    stamp=$(cat "$SY"/noreturn.json "$SY"/bodies.json "$SY"/returns.json "$SY"/fparams.json \
            tools/ghidra/albw_fix_*.py | sha1sum | cut -c1-16)
    if [[ -f $OUT/project.stamp && $(cat "$OUT/project.stamp") == "$stamp" ]]; then
        step "project: already fixed with these inputs (build/systemic/project.stamp)"
    else
        bak=$PROJ.bak_systemic_$(date +%Y%m%d)
        if ! ls -d "$PROJ".bak_systemic_* >/dev/null 2>&1; then
            step "project: backup -> $bak"
            cp -a "$PROJ" "$bak"
        fi
        step "project: applying the fixes (logs in build/systemic/*_log.json)"
        "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -scriptPath "$P/tools/ghidra" \
            -postScript albw_fix_noreturn.py "$SY/noreturn.json" "$OUT/noreturn_log.json" \
            -postScript albw_fix_bodies.py "$SY/bodies.json" "$P/native/robust/tools/livein.json" "$OUT/bodies_log.json" \
            -postScript albw_fix_returns.py "$SY/returns.json" "$SY/fparams.json" "$OUT/returns_log.json" \
            > "$LOGS/project.log" 2>&1
        grep -E "^albw_fix_" "$LOGS/project.log"
        grep -q "^albw_fix_returns:" "$LOGS/project.log" || { step "project: a fix script failed (see $LOGS/project.log)"; exit 1; }
        echo "$stamp" > "$OUT/project.stamp"
    fi
fi

# ---------------------------------------------------------------- 3. dump
if want dump; then
    [[ -f $OUT/bodies_log.json ]] || { step "dump: build/systemic/bodies_log.json missing (run the project step)"; exit 1; }
    step "dump: ghidra_dump.py (V2) -> build/ghidra_analyzed3 in $SHARDS shards"
    rm -rf "$DUMP.tmp" "$OUT/shards"
    mkdir -p "$DUMP.tmp" "$OUT/shards"
    echo '{}' > "$OUT/idx_none.json"
    python3 - "$D/funcs.json" "$OUT/shards" "$SHARDS" <<'EOF'
import json, sys
starts = json.load(open(sys.argv[1]))["starts"]
n = int(sys.argv[3])
for k in range(n):
    json.dump({"starts": starts[k::n]}, open("%s/shard%d.json" % (sys.argv[2], k), "w"))
EOF
    pids=()
    for ((k = 0; k < SHARDS; k++)); do
        cp -a "$PROJ" "$OUT/shards/proj$k"
        rm -f "$OUT/shards/proj$k/ALBW_EU_FULL.lock"*
        ( cd "$D" && ALBW_DUMP_V2=1 ALBW_REMOVED_STARTS="$OUT/bodies_log.json" \
            "$GH" "$OUT/shards/proj$k" ALBW_EU_FULL -process -noanalysis -readOnly \
            -scriptPath "$D/tools;$P/tools/ghidra" \
            -postScript ghidra_dump.py "$DUMP.tmp" "$OUT/shards/shard$k.json" "$OUT/idx_none.json" shared/eu_usa_map.json \
            > "$LOGS/dump$k.log" 2>&1 ) &
        pids+=($!)
    done
    fail=0
    for p in "${pids[@]}"; do wait "$p" || fail=1; done
    grep -h "^ghidra_dump: \(done\|V2\)" "$LOGS"/dump*.log || true
    n=$(ls "$DUMP.tmp" | wc -l)
    want_n=$(python3 -c "import json;print(len(json.load(open('$D/funcs.json'))['starts']))")
    if ((fail)) || ((n * 100 < want_n * 99)); then
        step "dump: incomplete ($n of $want_n files; see $LOGS/dump*.log)"; exit 1
    fi
    rm -rf "$DUMP" "$OUT/shards"
    mv "$DUMP.tmp" "$DUMP"
    step "dump: $n of $want_n functions in build/ghidra_analyzed3"
fi

# ---------------------------------------------------------------- 4. extra
if want extra; then
    step "extra: albw_dump_extra.py (V2) -> build/ghidra_extra3"
    rm -rf "$EXTRA" && mkdir -p "$EXTRA"
    ( cd "$D" && ALBW_DUMP_V2=1 "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -scriptPath "$P/tools/ghidra" \
        -postScript albw_dump_extra.py shared/eu_usa_map.json "$EXTRA" "$OUT/extra_functions.json" "$P/build/ptr_targets.json" \
        > "$LOGS/extra.log" 2>&1 )
    grep -h "^albw_dump_extra" "$LOGS/extra.log"
    [[ -s $OUT/extra_functions.json ]] || { step "extra: no map written (see $LOGS/extra.log)"; exit 1; }
    # gen_data (link.sh) names code pointers from native/extra_functions.json: functions the new
    # dump adds must be known there too (union; the old file is kept in the backup)
    python3 - "$P/native/extra_functions.json" "$OUT/extra_functions.json" "$OUT/backup" <<'EOF'
import json, os, shutil, sys
old_p, new_p, bak = sys.argv[1:4]
old = json.load(open(old_p)) if os.path.exists(old_p) else {}
new = json.load(open(new_p))
add = {k: v for k, v in new.items() if k not in old}
gone = sorted(k for k in old if k not in new)
print("extra functions: %d now, %d before; %d new, %d no longer functions" % (len(new), len(old), len(add), len(gone)))
if add:
    os.makedirs(bak, exist_ok=True)
    if not os.path.exists(os.path.join(bak, "extra_functions.json")):
        shutil.copy2(old_p, os.path.join(bak, "extra_functions.json"))
    old.update(add)
    json.dump(old, open(old_p, "w"), indent=0, sort_keys=True)
    print("native/extra_functions.json: %d added (backup in build/systemic/backup)" % len(add))
EOF
fi

# ---------------------------------------------------------------- 5. build
export ALBW_CONVERT_V2=1 ALBW_GHIDRA_DIR=$DUMP ALBW_GHIDRA_EXTRA_DIR=$EXTRA ALBW_EXTRA_FUNCTIONS=$OUT/extra_functions.json
if want build; then
    step "build: tools/build_decomp.py (V2, build/ghidra_analyzed3)"
    python3 tools/build_decomp.py > "$LOGS/build_decomp.log" 2>&1
    grep -E "^(Ghidra dump|sources|functions exported|unresolved)" "$LOGS/build_decomp.log"
    echo "float bit-casts rewritten: $(grep -rho 'ALBW_AS_[FU]' build/decomp/ghidra_fallback/ghidra_analyzed3 | wc -l)" \
         "in $(grep -rl 'ALBW_AS_[FU]' build/decomp/ghidra_fallback/ghidra_analyzed3 | wc -l) files"
fi

# ---------------------------------------------------------------- 6. names
if want names; then
    step "names: tools/resolve_names.py"
    python3 tools/resolve_names.py > "$LOGS/resolve_names.log" 2>&1
    tail -2 "$LOGS/resolve_names.log"
fi

# ---------------------------------------------------------------- 7. ctors
if want ctors; then
    step "ctors: tools/ctor_regen.py"
    "$PY" tools/ctor_regen.py --backup "$OUT/backup/native_game" > "$LOGS/ctor_regen.log" 2>&1
    cat "$LOGS/ctor_regen.log"
fi

# ---------------------------------------------------------------- 8. link
if want link; then
    step "link: tools/link.sh"
    tools/link.sh > "$LOGS/link.log" 2>&1
    tail -1 "$LOGS/link.log"
fi
step "systemic rebuild done (logs: build/systemic/logs)"
