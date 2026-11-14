#!/usr/bin/env bash
# AMM ILS and SA on the 150 large instances (abs{1..10}n{50,100,200}_{1..5}), 5 seeds, 60 s each.
# Runs strictly sequentially (1 CPU core). Order: instance -> algorithm -> seed, so partial
# results stay balanced between ILS and SA. Resumable: runs already in the CSV are skipped.
#
# usage: scripts/run_large.sh [time_limit_s=60] [seeds=5] [out=results/large_60s.csv]
set -u
cd "$(dirname "$(readlink -f "$0")")/.."
T=${1:-60}; SEEDS=${2:-5}; OUT=${3:-results/large_60s.csv}
DATA=../Data/Large
mkdir -p "$(dirname "$OUT")" results/sol
for n in 50 100 200; do for k in 1 2 3 4 5; do for a in $(seq 1 10); do
    inst=abs${a}n${n}_${k}
    for algo in ils sa; do for s in $(seq 1 "$SEEDS"); do
        if [ -f "$OUT" ] && grep -q "^${inst},.*,${algo},lr,${s},${T}," "$OUT"; then continue; fi
        echo "$(date '+%F %T') $inst $algo seed $s"
        ./amm "$DATA/$inst.dat" --algo "$algo" --obj lr --time "$T" --seed "$s" \
            --csv "$OUT" --sol "results/sol/${inst}_${algo}_s${s}.txt" > /dev/null 2>&1 \
            || echo "$(date '+%F %T') FAILED $inst $algo seed $s"
    done; done
done; done; done
echo "$(date '+%F %T') ALL DONE"
