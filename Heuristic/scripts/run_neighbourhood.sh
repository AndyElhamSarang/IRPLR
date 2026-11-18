#!/usr/bin/env bash
# MSILS neighbourhood size test: MaxShift1 in {1,2,3} x MaxSwap1 in {1,2} x MaxSwap2 in {1,2}
# (12 configurations) on the instances of Data/Large/test.dat, compared with AMM ILS/SA
# (AMM/results/large_60s.csv, same seeds 1..5 and time limit). All other parameters as in
# par/GlobalParameter.txt. Loop order seed > instance > configuration, so every finished seed
# gives a complete grid. Runs strictly sequentially (1 CPU core). Resumable: runs already in
# the CSV are skipped.
#
# usage: scripts/run_neighbourhood.sh [time_limit_s=60] [seeds=5] [out=results/neighbourhood_test_60s.csv] [instance_list=../Data/Large/test.dat] [data_dir=folder of instance_list]
set -u
cd "$(dirname "$(readlink -f "$0")")/.."
T=${1:-60}; SEEDS=${2:-5}; OUT=${3:-results/neighbourhood_test_60s.csv}
LIST=${4:-../Data/Large/test.dat}
DATA=${5:-$(dirname "$(readlink -f "$LIST")")}
EXE=src/MS_parameter_tuning.out
mkdir -p "$(dirname "$OUT")"
[ -f "$OUT" ] || echo "instance,max_shift1,max_swap1,max_swap2,seed,time_limit,logistic_ratio,iterations,starts,elapsed,status" > "$OUT"
PARAMS=(--NumberOfExperiments 1 --NumberOfInitialSolutions 100 --InitialSolutionMethod Initial_BlockZone_Schedule
        --MinGridResolution 3 --GridResolutionIncrement 2 --ActivateHGS NO
        --theta 0.62 --sigma 5 --phi 16
        --AllowLagrangianRelaxation YES --InitialLagrangianScalar 5 --ToTriggerAdjustment 5 --ToAdjustLagrangianScalar 1
        --LocalSearchOrder SEQUENTIAL --LocalSearchMemory EXACT --UseVehicleSlack YES
        --TypeOfRebalance GUARANTEE_FEASIBILITY --Activate_Disturbance SEGMENT --DisturbStrength 2
        --MinShift1 1 --MinShift2 0 --MaxShift2 0 --MinSwap1 1 --MinSwap2 1
        --MinRemoveLength 0 --MaxRemoveLength 1 --MinInsertLength 0 --MaxInsertLength 1
        --MainAlgorithmTimeLimit "$T")
TMP=$(mktemp)
trap 'rm -f "$TMP"' EXIT
mapfile -t INSTANCES < <(grep -v '^\s*#' "$LIST" | grep . | tr -d '\r')
for s in $(seq 1 "$SEEDS"); do
  for file in "${INSTANCES[@]}"; do
    inst=$(basename "$file" .dat)
    for sh1 in 1 2 3; do for sw1 in 1 2; do for sw2 in 1 2; do
        if grep -q "^${inst},${sh1},${sw1},${sw2},${s},${T}," "$OUT"; then continue; fi
        echo "$(date '+%F %T') $inst shift1<=$sh1 swap1<=$sw1 swap2<=$sw2 seed $s"
        if [ ! -x "$EXE" ]; then echo "$(date '+%F %T') ABORT: $EXE missing; rerun to resume"; exit 1; fi
        "$EXE" --instance "$DATA/$file" --seed "$s" --MaxShift1 "$sh1" --MaxSwap1 "$sw1" --MaxSwap2 "$sw2" "${PARAMS[@]}" > "$TMP" 2>&1 < /dev/null
        rc=$?
        if [ $rc -eq 126 ] || [ $rc -eq 127 ]; then echo "$(date '+%F %T') ABORT: cannot execute $EXE (exit $rc); rerun to resume"; exit 1; fi
        get() { grep "^$1 " "$TMP" | tail -n 1 | awk '{print $2}'; }
        lr=$(get IRACE_COST)
        if [ $rc -eq 0 ] && [ -n "$lr" ]; then status=ok; else status="fail$rc"; echo "$(date '+%F %T') FAILED"; tail -5 "$TMP"; fi
        echo "$inst,$sh1,$sw1,$sw2,$s,$T,$lr,$(get IRACE_ITERATIONS),$(get IRACE_STARTS),$(get IRACE_ELAPSED),$status" >> "$OUT"
    done; done; done
  done
done
echo "$(date '+%F %T') ALL DONE"
