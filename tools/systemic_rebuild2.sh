#!/usr/bin/env bash
# Systemic rebuild, round 2: parameter counts and return types from the ARM code applied in the
# Ghidra project (one signature table for callers and callees), re-dump, convert with V2+V3, the
# matched-source ABI checks, rebuild and link. Nothing here runs by itself; start it by hand at a
# boot-agent pause point:
#
#   tools/systemic_rebuild2.sh                 # everything (~20-30 min)
#   SHARDS=4 tools/systemic_rebuild2.sh        # fewer parallel Ghidra dumps (RAM: ~3 GB each)
#   FROM=dump tools/systemic_rebuild2.sh       # restart at a step (prep project dump extra build names ctors link)
#
# Steps (each logged in build/systemic2/logs/<step>.log, overall log build/systemic2/rebuild.log):
#   1. prep      native/robust/tools/livein2.py (precise live-in: corrected capstone register
#                access, indirect calls, tail/fall-through into foreign code, call-site and
#                virtual-sibling evidence) -> livein2.json; the project's current signatures
#                (albw_export_sigs.py, read-only) -> build/systemic2/sigs_before.json;
#                tools/systemic_prep2.py -> native/robust/systemic2/{params,returns}.json.
#                Skipped when native/robust/systemic2/params.json exists (delete it to redo).
#   2. project   backup logs/ghidra_full -> logs/ghidra_full.bak_systemic2_<date> (once), then
#                albw_fix_params.py (missing register parameters added, proven-unread ones
#                removed) and albw_fix_returns.py with the round-2 returns (idempotent; skipped
#                when build/systemic2/project.stamp matches the inputs); signatures after ->
#                build/systemic2/sigs_after.json
#   3. dump      ghidra_dump.py (ALBW_DUMP_V2=1 ALBW_DUMP_V3=1: + "// PARAM" stack-parameter
#                lines) -> build/ghidra_analyzed4, $SHARDS parallel processes on project copies
#   4. extra     albw_dump_extra.py (V2+V3) -> build/ghidra_extra4 + its USA->EU map
#   5. build     tools/build_decomp.py with ALBW_CONVERT_V2=1 ALBW_CONVERT_V3=1 (stack0x objects
#                in the frame, wrapped signatures parsed, extra-dump prototypes),
#                ALBW_MATCHED_ARGCHECK=1 ALBW_MATCHED_ABICHECK=1 (matched sources that drop
#                register arguments / disagree on the ARM layout -> Ghidra version), dump
#                build/ghidra_analyzed4 (sources under build/decomp/ghidra_fallback/ghidra_analyzed4)
#   6. names     tools/resolve_names.py
#   7. ctors     tools/ctor_regen.py (generated constructors only; hand-written files kept)
#   8. link      tools/link.sh
#
# Rollback: tools/build_decomp_v2.sh && tools/link.sh (round-1 build from build/ghidra_analyzed3,
# which this script never touches); the project backup is logs/ghidra_full.bak_systemic2_<date>
# (round-1 backups: logs/ghidra_full.bak_systemic_<date>, logs/ghidra_full.bak_20261114).
set -euo pipefail
cd "$(dirname "$0")/.."
P=$PWD
D=$(cd "$P/../albw-decomp-ai" && pwd)
GH=${GHIDRA_HEADLESS:-<your Ghidra 12.0.2 install>/support/analyzeHeadless}
PY=${ALBW_PY:-$D/.venv/bin/python}
PROJ=$D/logs/ghidra_full
SY=$P/native/robust/systemic2
OUT=$P/build/systemic2
LOGS=$OUT/logs
DUMP=$P/build/ghidra_analyzed4
EXTRA=$P/build/ghidra_extra4
REMOVED=$P/build/systemic/bodies_log.json   # round 1: starts removed from the project
SHARDS=${SHARDS:-6}
FROM=${FROM:-prep}
mkdir -p "$LOGS"
exec > >(tee -a "$OUT/rebuild.log") 2>&1
exec 9>"$OUT/.lock"
flock -n 9 || { echo "another systemic_rebuild2.sh is running"; exit 1; }

step() { echo "[$(date '+%F %T')] $*"; }
order=(prep project dump extra build names ctors link)
want() {  # run step $1 if it is at or after $FROM
    local s seen=0
    for s in "${order[@]}"; do [[ $s == "$FROM" ]] && seen=1; [[ $s == "$1" ]] && { ((seen)); return; }; done
    return 1
}

step "systemic rebuild 2: from=$FROM shards=$SHARDS"
if [[ -e "$PROJ/ALBW_EU_FULL.lock" ]]; then
    step "Ghidra project is open elsewhere ($PROJ/ALBW_EU_FULL.lock); close it first"; exit 1
fi
[[ -f $REMOVED ]] || { step "round-1 log $REMOVED missing (run tools/systemic_rebuild.sh first)"; exit 1; }

export_sigs() {  # $1 = output json; read-only, on a private copy (Ghidra locks even read-only)
    rm -rf "$OUT/sigproj"; mkdir -p "$OUT/sigproj"
    cp -a "$PROJ"/ALBW_EU_FULL.gpr "$PROJ"/ALBW_EU_FULL.rep "$OUT/sigproj/"
    "$GH" "$OUT/sigproj" ALBW_EU_FULL -process -noanalysis -readOnly -scriptPath "$P/tools/ghidra" \
        -postScript albw_export_sigs.py "$1" > "$LOGS/export_sigs.log" 2>&1
    rm -rf "$OUT/sigproj"
    [[ -s $1 ]] || { step "signature export failed (see $LOGS/export_sigs.log)"; exit 1; }
}

# ---------------------------------------------------------------- 1. prep
if want prep; then
    LI=$P/native/robust/tools/livein2.json
    # stale inputs force a recompute: livein2.py (or its helpers) newer than livein2.json, or
    # livein2.json newer than the params/returns derived from it
    if [[ -s $LI ]] && [[ -n $(find "$P/native/robust/tools" -name '*.py' -newer "$LI") ]]; then
        step "prep: livein2.py changed since livein2.json, recomputing"
        "$PY" native/robust/tools/livein2.py > "$LOGS/livein2.log" 2>&1
        grep -v -e Warning -e iteration "$LOGS/livein2.log" | tail -4
    fi
    # (no automatic params/returns recompute: against an already-fixed project it yields an
    #  almost empty table; round 3 snapshots the signatures before fixing instead)
    if [[ -s $SY/params.json && -s $SY/returns.json && -s $LI ]]; then
        step "prep: using native/robust/systemic2/*.json (delete params.json to recompute)"
    else
        step "prep: livein2.py"
        "$PY" native/robust/tools/livein2.py > "$LOGS/livein2.log" 2>&1
        grep -v -e Warning -e iteration "$LOGS/livein2.log" | tail -4
        step "prep: Ghidra signatures before the fix"
        export_sigs "$OUT/sigs_before.json"
        step "prep: systemic_prep2.py"
        "$PY" tools/systemic_prep2.py "$OUT/sigs_before.json" > "$LOGS/prep2.log" 2>&1
        grep -v Warning "$LOGS/prep2.log"
    fi
fi

# ---------------------------------------------------------------- 2. project
if want project; then
    stamp=$(cat "$SY"/params.json "$SY"/returns.json tools/ghidra/albw_fix_params.py \
            tools/ghidra/albw_fix_returns.py | sha1sum | cut -c1-16)
    if [[ -f $OUT/project.stamp && $(cat "$OUT/project.stamp") == "$stamp" ]]; then
        step "project: already fixed with these inputs (build/systemic2/project.stamp)"
    else
        bak=$PROJ.bak_systemic2_$(date +%Y%m%d)
        if ! ls -d "$PROJ".bak_systemic2_* >/dev/null 2>&1; then
            step "project: backup -> $bak"
            cp -a "$PROJ" "$bak"
        fi
        [[ -f $SY/fparams_none.json ]] || echo '{}' > "$SY/fparams_none.json"
        step "project: applying the fixes (logs in build/systemic2/*_log.json)"
        "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -scriptPath "$P/tools/ghidra" \
            -postScript albw_fix_params.py "$SY/params.json" "$OUT/params_log.json" \
            -postScript albw_fix_returns.py "$SY/returns.json" "$SY/fparams_none.json" "$OUT/returns_log.json" \
            > "$LOGS/project.log" 2>&1
        grep -E "^albw_fix_" "$LOGS/project.log"
        grep -q "^albw_fix_returns:" "$LOGS/project.log" || { step "project: a fix script failed (see $LOGS/project.log)"; exit 1; }
        echo "$stamp" > "$OUT/project.stamp"
        export_sigs "$OUT/sigs_after.json"
    fi
fi

# ---------------------------------------------------------------- 3. dump
if want dump; then
    step "dump: ghidra_dump.py (V2+V3) -> build/ghidra_analyzed4 in $SHARDS shards"
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
        mkdir -p "$OUT/shards/proj$k"
        cp -a "$PROJ"/ALBW_EU_FULL.gpr "$PROJ"/ALBW_EU_FULL.rep "$OUT/shards/proj$k/"
        ( cd "$D" && ALBW_DUMP_V2=1 ALBW_DUMP_V3=1 ALBW_DECOMP_TIMEOUT=${ALBW_DECOMP_TIMEOUT:-120} ALBW_REMOVED_STARTS="$REMOVED" \
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
    step "dump: $n of $want_n functions in build/ghidra_analyzed4"
fi

# ---------------------------------------------------------------- 4. extra
if want extra; then
    step "extra: albw_dump_extra.py (V2+V3) -> build/ghidra_extra4"
    rm -rf "$EXTRA" && mkdir -p "$EXTRA"
    ( cd "$D" && ALBW_DUMP_V2=1 ALBW_DUMP_V3=1 "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -readOnly \
        -scriptPath "$P/tools/ghidra" \
        -postScript albw_dump_extra.py shared/eu_usa_map.json "$EXTRA" "$OUT/extra_functions.json" "$P/build/ptr_targets.json" \
        > "$LOGS/extra.log" 2>&1 )
    grep -h "^albw_dump_extra" "$LOGS/extra.log"
    [[ -s $OUT/extra_functions.json ]] || { step "extra: no map written (see $LOGS/extra.log)"; exit 1; }
    python3 - "$P/native/extra_functions.json" "$OUT/extra_functions.json" "$OUT/backup" <<'EOF'
import json, os, shutil, sys
old_p, new_p, bak = sys.argv[1:4]
old = json.load(open(old_p)) if os.path.exists(old_p) else {}
new = json.load(open(new_p))
add = {k: v for k, v in new.items() if k not in old}
print("extra functions: %d now, %d before; %d new" % (len(new), len(old), len(add)))
if add:
    os.makedirs(bak, exist_ok=True)
    if not os.path.exists(os.path.join(bak, "extra_functions.json")):
        shutil.copy2(old_p, os.path.join(bak, "extra_functions.json"))
    old.update(add)
    json.dump(old, open(old_p, "w"), indent=0, sort_keys=True)
    print("native/extra_functions.json: %d added (backup in build/systemic2/backup)" % len(add))
EOF
fi

# ---------------------------------------------------------------- 5. build
export ALBW_CONVERT_V2=1 ALBW_CONVERT_V3=1 ALBW_MATCHED_ARGCHECK=1 ALBW_MATCHED_ABICHECK=1 \
       ALBW_GHIDRA_DIR=$DUMP ALBW_GHIDRA_EXTRA_DIR=$EXTRA ALBW_EXTRA_FUNCTIONS=$OUT/extra_functions.json
if want build; then
    step "build: tools/build_decomp.py (V2+V3, matched ABI checks, build/ghidra_analyzed4)"
    python3 tools/build_decomp.py > "$LOGS/build_decomp.log" 2>&1
    grep -E "^(Ghidra dump|matched ABI|fallback to|sources|functions exported|unresolved)" "$LOGS/build_decomp.log"
    G=build/decomp/ghidra_fallback/ghidra_analyzed4
    echo "converted Ghidra sources: in_rN in $(grep -rlE '\bin_r[0-3]\b' $G | wc -l) files," \
         "in_sN in $(grep -rlE '\bin_s[0-9]+\b' $G | wc -l), stack0x statics in" \
         "$(grep -rl 'caller stack slot, fix by hand' $G | wc -l), stack0x frame slots in $(grep -rl '^#define stack0x' $G | wc -l)"
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
step "systemic rebuild 2 done (logs: build/systemic2/logs)"
