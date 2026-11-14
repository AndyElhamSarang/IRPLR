#!/usr/bin/env bash
# AMM ILS and SA on the small instances of H3, H4, H5 and H6, in the order of Data/H*/all_instances.dat,
# 5 seeds, 30 s each. Runs strictly sequentially (1 CPU core). Order: horizon -> instance -> algorithm
# -> seed. One CSV per horizon (instance names repeat across horizons). Resumable: runs already in the
# CSV are skipped.
#
# usage: scripts/run_small.sh [time_limit_s=30] [seeds=5] [horizons="H3 H4 H5 H6"]
set -u
cd "$(dirname "$(readlink -f "$0")")/.."
T=${1:-30}; SEEDS=${2:-5}; HORIZONS=${3:-"H3 H4 H5 H6"}
if [ ! -x ./amm ]; then echo "$(date '+%F %T') ABORT: ./amm missing"; exit 1; fi
for h in $HORIZONS; do
    DATA=../Data/$h
    OUT=results/${h}_${T}s.csv
    mkdir -p results "results/sol_${h}"
    while read -r file; do
        inst=$(basename "$file" .dat)
        for algo in ils sa; do for s in $(seq 1 "$SEEDS"); do
            if [ -f "$OUT" ] && grep -q "^${inst},.*,${algo},lr,${s},${T}," "$OUT"; then continue; fi
            echo "$(date '+%F %T') $h $inst $algo seed $s"
            ./amm "$DATA/$inst.dat" --algo "$algo" --obj lr --time "$T" --seed "$s" \
                --csv "$OUT" --sol "results/sol_${h}/${inst}_${algo}_s${s}.txt" > /dev/null 2>&1 < /dev/null \
                || echo "$(date '+%F %T') FAILED $h $inst $algo seed $s"
        done; done
    done < <(tr -d '\r' < "$DATA/all_instances.dat" | grep -v '^\s*#' | grep .)
done
echo "$(date '+%F %T') ALL DONE"
