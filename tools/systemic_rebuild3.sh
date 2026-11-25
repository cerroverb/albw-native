#!/usr/bin/env bash
# Systemic rebuild, round 3 (native/ROUND3.md): ARM return classes, vpush {d0} float arguments,
# 64-bit returns, variadic calls, vldm pair loads, returns-this list, stack objects whose stores
# Ghidra dropped, in_stack_ aliasing, the compile-fix tooling (native/compilefix/tooling.patch,
# integrated under ALBW_CONVERT_V4). Nothing here runs by itself; start it by hand at a
# boot-agent pause point:
#
#   tools/systemic_rebuild3.sh                 # everything (~25-35 min)
#   SHARDS=4 tools/systemic_rebuild3.sh        # fewer parallel Ghidra dumps (RAM: ~3 GB each)
#   FROM=dump tools/systemic_rebuild3.sh       # restart at a step (prep project dump extra build names ctors link)
#   UNTIL=build tools/systemic_rebuild3.sh     # stop after a step (e.g. no link)
#   STAGE=1 tools/systemic_rebuild3.sh         # stage 1 only (default STAGE=2 = everything):
#        return classes (retfix), returns-this (incl. loop constructors), stack objects +
#        in_stack_ aliasing, FUN_ callee declarations. Stage 2 adds the vpush float arguments,
#        variadic calls, 64-bit returns, pair loads, compile fixes, FCONV, switch fragments;
#        after a stage-1 run: STAGE=2 FROM=project tools/systemic_rebuild3.sh
#
# Steps (each logged in build/systemic3/logs/<step>.log, overall log build/systemic3/rebuild.log):
#   1. prep      tools/livein3.py (livein2 + vpush {d0} / d-register float arguments) ->
#                native/robust/tools/livein3.json; tools/retclass3.py (return class of every
#                function from the ARM code) -> native/robust/systemic3/retclass.json; the
#                project's signatures BEFORE the round-3 fix (albw_export_sigs.py, read-only) ->
#                build/systemic3/sigs_before.json (exported once and kept: the tables are a delta
#                against the project as it was before round 3); tools/systemic_prep3.py ->
#                native/robust/systemic3/{params,returns,retfix,varargs,stackobjs,report}.json +
#                native/returns_this3.json.
#                Stale-table checks: livein3.json is recomputed when livein2.json, livein2.py,
#                livein.py, livein_dregs.py or tools/livein3.py is newer; retclass.json when
#                tools/retclass3.py, livein*.py or retreads.json is newer; the systemic3 tables
#                when livein3.json, retclass.json, sigs_before.json or systemic_prep3.py is newer.
#                If the project was already fixed (build/systemic3/project.stamp) the tables are
#                recomputed from the saved sigs_before.json, never from the fixed project (round
#                2's re-run against its own fixed project emptied systemic2/params.json).
#   2. project   backup logs/ghidra_full -> logs/ghidra_full.bak_systemic3_<date> (once), then
#                albw_fix_params.py (systemic3/params.json: the 17 vpush/d-register float
#                arguments), albw_fix_returns.py (systemic3/returns.json: void functions that
#                return their first argument on every path, loops included), albw_fix_varargs.py
#                (systemic3/varargs.json), albw_fix_stackobjs.py (systemic3/stackobjs.json);
#                idempotent, skipped when build/systemic3/project.stamp matches the inputs;
#                signatures after -> build/systemic3/sigs_after.json
#   3. dump      ghidra_dump.py (V2+V3) -> build/ghidra_analyzed5 ($SHARDS parallel project copies)
#   4. extra     albw_dump_extra.py (V2+V3) -> build/ghidra_extra5
#   5. build     tools/build_decomp.py with ALBW_CONVERT_V2/V3/V4=1 (ALBW_RETFIX =
#                systemic3/retfix.json, returns_this3.json, FCONV on), ALBW_MATCHED_ARGCHECK/
#                ABICHECK=1, dump build/ghidra_analyzed5 (sources under
#                build/decomp/ghidra_fallback/ghidra_analyzed5_v4)
#   6. names     tools/resolve_names.py
#   7. ctors     tools/ctor_regen.py
#   8. link      tools/link.sh (unchanged: it thunks the __impl symbols build_decomp exports for
#                the returns-this list, which under V4 is native/returns_this3.json)
#
# 64-bit: after step 5, tools/build_decomp64_v4.sh && tools/link64.sh (same converter flags).
# Rollback: tools/build_decomp_v3.sh && tools/link.sh (round-2 build from build/ghidra_analyzed4,
# which this script never touches); the project backup is logs/ghidra_full.bak_systemic3_<date>.
set -euo pipefail
cd "$(dirname "$0")/.."
P=$PWD
D=$(cd "$P/../albw-decomp-ai" && pwd)
GH=${GHIDRA_HEADLESS:-<your Ghidra 12.0.2 install>/support/analyzeHeadless}
PY=${ALBW_PY:-$D/.venv/bin/python}
PROJ=$D/logs/ghidra_full
SY=$P/native/robust/systemic3
OUT=$P/build/systemic3
LOGS=$OUT/logs
DUMP=$P/build/ghidra_analyzed5
EXTRA=$P/build/ghidra_extra5
PREV_DUMP=$P/build/ghidra_analyzed4          # round 2: the &stack0x objects are read from it
REMOVED=$P/build/systemic/bodies_log.json   # round 1: starts removed from the project
SHARDS=${SHARDS:-6}
STAGE=${STAGE:-2}
FROM=${FROM:-prep}
UNTIL=${UNTIL:-link}
mkdir -p "$LOGS" "$SY"
exec > >(tee -a "$OUT/rebuild.log") 2>&1
exec 9>"$OUT/.lock"
flock -n 9 || { echo "another systemic_rebuild3.sh is running"; exit 1; }

step() { echo "[$(date '+%F %T')] $*"; }
order=(prep project dump extra build names ctors link)
want() {  # run step $1 if it is between $FROM and $UNTIL
    local s seen=0 done_=0
    for s in "${order[@]}"; do
        [[ $s == "$FROM" ]] && seen=1
        if [[ $s == "$1" ]]; then ((seen && !done_)); return; fi
        [[ $s == "$UNTIL" ]] && done_=1
    done
    return 1
}
newer() {  # newer TARGET SRC...: true if TARGET is missing/empty or any SRC is newer
    local t=$1; shift
    [[ -s $t ]] || return 0
    local s; for s in "$@"; do [[ $s -nt $t ]] && return 0; done
    return 1
}

step "systemic rebuild 3: stage=$STAGE from=$FROM until=$UNTIL shards=$SHARDS"
if [[ -e "$PROJ/ALBW_EU_FULL.lock" ]]; then
    step "Ghidra project is open elsewhere ($PROJ/ALBW_EU_FULL.lock); close it first"; exit 1
fi
[[ -f $REMOVED ]] || { step "round-1 log $REMOVED missing"; exit 1; }
[[ -d $PREV_DUMP ]] || { step "round-2 dump $PREV_DUMP missing (tools/systemic_rebuild2.sh)"; exit 1; }

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
    T=$P/native/robust/tools
    LI2=$T/livein2.json LI3=$T/livein3.json RC=$SY/retclass.json
    if [[ ! -s $LI2 ]]; then
        step "prep: livein2.json missing, native/robust/tools/livein2.py"
        "$PY" "$T/livein2.py" > "$LOGS/livein2.log" 2>&1
    fi
    if newer "$LI3" "$LI2" "$T/livein2.py" "$T/livein.py" "$P/native/gameplay/tools/livein_dregs.py" tools/livein3.py; then
        step "prep: tools/livein3.py (vpush {d0} / d-register float arguments)"
        "$PY" tools/livein3.py > "$LOGS/livein3.log" 2>&1
        grep -v -e Warning "$LOGS/livein3.log" | tail -2
    fi
    if newer "$RC" tools/retclass3.py "$T/livein2.py" "$T/livein.py" "$P/native/gameplay/tools/retreads.json" "$P/build/ptr_targets.json"; then
        step "prep: tools/retclass3.py (return classes from the ARM code)"
        "$PY" tools/retclass3.py > "$LOGS/retclass3.log" 2>&1
        grep -v -e Warning "$LOGS/retclass3.log" | tail -1
    fi
    if [[ ! -s $OUT/sigs_before.json ]]; then
        if [[ -f $OUT/project.stamp ]]; then
            step "prep: project already fixed by round 3 but build/systemic3/sigs_before.json is gone;" \
                 "restore it (or the project backup) before recomputing the tables"; exit 1
        fi
        step "prep: Ghidra signatures before the round-3 fix -> build/systemic3/sigs_before.json"
        export_sigs "$OUT/sigs_before.json"
    fi
    if newer "$SY/retfix.json" "$LI3" "$RC" "$OUT/sigs_before.json" tools/systemic_prep3.py tools/systemic_prep2.py \
       || newer "$SY/params.json" "$LI3" "$OUT/sigs_before.json" tools/systemic_prep3.py \
       || newer "$SY/stackobjs.json" "$PREV_DUMP" tools/systemic_prep3.py; then
        if [[ -s $SY/params.json ]]; then
            bk=$SY/prev_$(date +%Y%m%d_%H%M%S); mkdir -p "$bk"
            cp -a "$SY"/*.json "$bk"/ 2>/dev/null || true
            step "prep: tables stale, previous ones kept in ${bk#$P/}"
        fi
        step "prep: tools/systemic_prep3.py (against build/systemic3/sigs_before.json)"
        "$PY" tools/systemic_prep3.py "$OUT/sigs_before.json" "$PREV_DUMP" > "$LOGS/prep3.log" 2>&1
        grep -v Warning "$LOGS/prep3.log"
    else
        step "prep: native/robust/systemic3/*.json up to date"
    fi
fi

# ---------------------------------------------------------------- 2. project
if want project; then
    G=$P/tools/ghidra
    if ((STAGE >= 2)); then
        PARAMS=$SY/params.json VARARGS=$SY/varargs.json
    else  # stage 1: no parameter / variadic changes
        echo '{}' > "$OUT/empty.json"; PARAMS=$OUT/empty.json VARARGS=$OUT/empty.json
    fi
    stamp=$(echo "stage$STAGE" | cat - "$PARAMS" "$SY"/returns.json "$VARARGS" "$SY"/stackobjs.json \
            "$G"/albw_fix_params.py "$G"/albw_fix_returns.py "$G"/albw_fix_varargs.py "$G"/albw_fix_stackobjs.py \
            | sha1sum | cut -c1-16)
    if [[ -f $OUT/project.stamp && $(cat "$OUT/project.stamp") == "$stamp" ]]; then
        step "project: already fixed with these inputs (build/systemic3/project.stamp)"
    else
        if ! ls -d "$PROJ".bak_systemic3_* >/dev/null 2>&1; then
            bak=$PROJ.bak_systemic3_$(date +%Y%m%d)
            step "project: backup -> $bak"
            cp -a "$PROJ" "$bak"
        fi
        [[ -f $SY/fparams_none.json ]] || echo '{}' > "$SY/fparams_none.json"
        step "project: applying the fixes (logs in build/systemic3/*_log.json)"
        "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -scriptPath "$G" \
            -postScript albw_fix_params.py "$PARAMS" "$OUT/params_log.json" \
            -postScript albw_fix_returns.py "$SY/returns.json" "$SY/fparams_none.json" "$OUT/returns_log.json" \
            -postScript albw_fix_varargs.py "$VARARGS" "$OUT/varargs_log.json" \
            -postScript albw_fix_stackobjs.py "$SY/stackobjs.json" "$OUT/stackobjs_log.json" \
            > "$LOGS/project.log" 2>&1
        grep -E "^albw_fix_" "$LOGS/project.log"
        grep -q "^albw_fix_stackobjs:" "$LOGS/project.log" || { step "project: a fix script failed (see $LOGS/project.log)"; exit 1; }
        echo "$stamp" > "$OUT/project.stamp"
        export_sigs "$OUT/sigs_after.json"
    fi
fi

# ---------------------------------------------------------------- 3. dump
if want dump; then
    step "dump: ghidra_dump.py (V2+V3) -> build/ghidra_analyzed5 in $SHARDS shards"
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
    step "dump: $n of $want_n functions in build/ghidra_analyzed5"
fi

# ---------------------------------------------------------------- 4. extra
if want extra; then
    step "extra: albw_dump_extra.py (V2+V3) -> build/ghidra_extra5"
    rm -rf "$EXTRA" && mkdir -p "$EXTRA"
    ( cd "$D" && ALBW_DUMP_V2=1 ALBW_DUMP_V3=1 "$GH" "$PROJ" ALBW_EU_FULL -process -noanalysis -readOnly \
        -scriptPath "$P/tools/ghidra" \
        -postScript albw_dump_extra.py shared/eu_usa_map.json "$EXTRA" "$OUT/extra_functions.json" "$P/build/ptr_targets.json" \
        > "$LOGS/extra.log" 2>&1 )
    grep -h "^albw_dump_extra" "$LOGS/extra.log"
    [[ -s $OUT/extra_functions.json ]] || { step "extra: no map written (see $LOGS/extra.log)"; exit 1; }
fi

# ---------------------------------------------------------------- 5. build
export ALBW_CONVERT_V2=1 ALBW_CONVERT_V3=1 ALBW_CONVERT_V4=1 ALBW_MATCHED_ARGCHECK=1 ALBW_MATCHED_ABICHECK=1 \
       ALBW_GHIDRA_DIR=$DUMP ALBW_GHIDRA_EXTRA_DIR=$EXTRA ALBW_EXTRA_FUNCTIONS=$OUT/extra_functions.json \
       ALBW_RETFIX=$SY/retfix.json ALBW_V4_STAGE=$STAGE
if want build; then
    step "build: tools/build_decomp.py (V2+V3+V4 stage $STAGE, matched ABI checks, build/ghidra_analyzed5)"
    python3 tools/build_decomp.py > "$LOGS/build_decomp.log" 2>&1
    grep -E "^(V4:|Ghidra dump|matched ABI|fallback to|sources|functions exported|unresolved)" "$LOGS/build_decomp.log"
    G=build/decomp/ghidra_fallback/ghidra_analyzed5_v4
    echo "converted Ghidra sources: in_rN in $(grep -rlE '\bin_r[0-3]\b' $G | wc -l) files," \
         "in_sN in $(grep -rlE '\bin_s[0-9]+\b' $G | wc -l), unaliased in_stack_ locals in" \
         "$(grep -rlE '^\s+\w[\w *]*\bin_stack_[0-9a-f]{8};' $G | wc -l), stack0x frame slots in $(grep -rl '^#define stack0x' $G | wc -l)"
    # native/framework/tools/lostframe.py --scan's count (it reads the round-2 directory): frame
    # slots of &stack0x objects never assigned in the C
    python3 - "$G" <<'EOF'
import glob, os, re, sys
n = ins = 0
for f in glob.glob(os.path.join(sys.argv[1], "*", "func_*.c")):
    t = open(f, errors="replace").read()
    names = re.findall(r"#define (stack0x\w+) ", t)
    if [x for x in names if not re.search(r"\b%s\s*(\[[^\]]*\])?\s*=[^=]" % x, t)]:
        n += 1
        ins += "in_stack_" in t
print("lostframe scan: %d functions with a never-assigned stack0x frame slot (%d with in_stack_ reads)" % (n, ins))
EOF
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
step "systemic rebuild 3 done (logs: build/systemic3/logs)"
