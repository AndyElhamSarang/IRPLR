#!/usr/bin/env bash
# Runs the AMM reimplementation on every instance of a directory, sequentially
# (runs must not share CPU cores, otherwise wall-clock time limits are not comparable).
#
# usage: scripts/run_batch.sh <instance_dir> <ils|sa> <time_limit_s> <n_seeds> <out.csv> [pattern]
# example (AMM protocol for OF2, best of 5 runs of 30 s):
#   scripts/run_batch.sh ../Data/H3 ils 30 5 results/H3_ils.csv
set -euo pipefail
dir=$1; algo=$2; time_limit=$3; seeds=$4; out=$5; pattern=${6:-*.dat}
bin="$(dirname "$0")/../amm"
mkdir -p "$(dirname "$out")"
for f in "$dir"/$pattern; do
    for ((s = 1; s <= seeds; s++)); do
        "$bin" "$f" --algo "$algo" --obj lr --time "$time_limit" --seed "$s" --csv "$out" || true
    done
done
