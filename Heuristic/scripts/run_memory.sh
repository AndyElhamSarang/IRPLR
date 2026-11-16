#!/usr/bin/env bash
# MSILS neighbourhood memory: NO (paper) vs ROUTE vs MIDDLE vs EXACT (see inc/LocalSearchMemory.h).
# Paired design: same instance and
# seed for both variants. Default instances: one held-out irace test instance per (n, K) class.
# Runs strictly sequentially (1 CPU core). Resumable: runs already in the CSV are skipped.
#
# usage: scripts/run_memory.sh [time_limit_s=60] [seeds=2] [out=results/memory_15_60s.csv] [instance_list]
set -u
cd "$(dirname "$(readlink -f "$0")")/.."
T=${1:-60}; SEEDS=${2:-2}; OUT=${3:-results/memory_15_60s.csv}
LIST=${4:-scripts/instances-15classes.txt}
EXE=src/MS_parameter_tuning.out
mkdir -p "$(dirname "$OUT")"
[ -f "$OUT" ] || echo "instance,order,seed,time_limit,logistic_ratio,iterations,starts,elapsed,status" > "$OUT"
PARAMS=(--NumberOfExperiments 1 --NumberOfInitialSolutions 30 --InitialSolutionMethod Initial_BlockZone_Schedule
        --MinGridResolution 3 --GridResolutionIncrement 2 --AllowLagrangianRelaxation YES
        --TypeOfRebalance NOT_GUARANTEE_FEASIBILITY --ActivateHGS NO --Activate_Disturbance YES
        --InitialLagrangianScalar 5 --ToAdjustLagrangianScalar 1 --ToTriggerAdjustment 5
        --theta 0.62 --sigma 5 --phi 16 --DisturbStrength 2 --MainAlgorithmTimeLimit "$T")
TMP=$(mktemp)
trap 'rm -f "$TMP"' EXIT
while read -r path; do
    inst=$(basename "$path" .dat)
    for s in $(seq 1 "$SEEDS"); do for order in NO ROUTE MIDDLE EXACT; do
        if grep -q "^${inst},${order},${s},${T}," "$OUT"; then continue; fi
        echo "$(date '+%F %T') $inst $order seed $s"
        # Stop (do not record a failure) if the binary is missing, e.g. while it is being rebuilt.
        if [ ! -x "$EXE" ]; then echo "$(date '+%F %T') ABORT: $EXE missing; rerun to resume"; exit 1; fi
        "$EXE" --instance "$path" --seed "$s" --LocalSearchMemory "$order" "${PARAMS[@]}" > "$TMP" 2>&1 < /dev/null
        rc=$?
        if [ $rc -eq 126 ] || [ $rc -eq 127 ]; then echo "$(date '+%F %T') ABORT: cannot execute $EXE (exit $rc); rerun to resume"; exit 1; fi
        get() { grep "^$1 " "$TMP" | tail -n 1 | awk '{print $2}'; }
        lr=$(get IRACE_COST)
        if [ $rc -eq 0 ] && [ -n "$lr" ]; then status=ok; else status="fail$rc"; echo "$(date '+%F %T') FAILED $inst $order seed $s"; fi
        echo "$inst,$order,$s,$T,$lr,$(get IRACE_ITERATIONS),$(get IRACE_STARTS),$(get IRACE_ELAPSED),$status" >> "$OUT"
    done; done
done < <(grep -v '^\s*#' "$LIST" | grep .)
echo "$(date '+%F %T') ALL DONE"
