#!/usr/bin/env python3
"""Compare the MSILS neighbourhood-size grid with AMM ILS/SA on the same instances.

usage: python3 scripts/compare_neighbourhood.py [results/neighbourhood_test_60s.csv] [../AMM/results/large_60s.csv]

Logistic ratio (lower is better). Per instance: mean over seeds for each MSILS configuration
(MaxShift1/MaxSwap1/MaxSwap2) and for AMM ILS and SA on the seeds present in both files,
plus mean MSILS iterations (to see how the neighbourhood size changes the search speed).
"""
import csv
import sys
from collections import defaultdict
from statistics import mean

msils_file = sys.argv[1] if len(sys.argv) > 1 else 'results/neighbourhood_test_60s.csv'
amm_file = sys.argv[2] if len(sys.argv) > 2 else '../AMM/results/large_60s.csv'

msils = defaultdict(dict)  # (instance, config) -> seed -> (lr, iterations)
for r in csv.DictReader(open(msils_file)):
    if r['status'] != 'ok':
        continue
    cfg = f"{r['max_shift1']}/{r['max_swap1']}/{r['max_swap2']}"
    msils[(r['instance'], cfg)][int(r['seed'])] = (float(r['logistic_ratio']), int(r['iterations'] or 0))

instances = sorted({i for i, _ in msils}, key=lambda x: (x.split('_')[0], int(x.split('_')[1])))
configs = sorted({c for _, c in msils})

amm = defaultdict(dict)  # (instance, algo) -> seed -> lr
for r in csv.DictReader(open(amm_file)):
    if r['instance'] in instances and r['objective'] == 'lr' and r['status'] == 'ok':
        amm[(r['instance'], r['algo'])][int(r['seed'])] = float(r['logistic_ratio'])

def fmt(x):
    return f'{x:.4f}' if x is not None else '   -  '

print('Mean logistic ratio over the seeds run so far (MSILS config = MaxShift1/MaxSwap1/MaxSwap2)')
print(f"{'instance':<14}{'seeds':>6}  " + '  '.join(f'{c:>7}' for c in configs) + f"  {'ILS':>7}  {'SA':>7}  best-MSILS-cfg  gap-to-best-AMM")
for inst in instances:
    seeds = set.intersection(*(set(msils[(inst, c)]) for c in configs if (inst, c) in msils))
    if not seeds:
        continue
    row = {c: mean(msils[(inst, c)][s][0] for s in seeds) for c in configs if (inst, c) in msils}
    a = {alg: (mean(amm[(inst, alg)][s] for s in seeds if s in amm[(inst, alg)]) if amm[(inst, alg)] else None) for alg in ('ils', 'sa')}
    best_cfg = min(row, key=row.get)
    best_amm = min(v for v in a.values() if v is not None)
    gap = 100 * (row[best_cfg] - best_amm) / best_amm
    print(f'{inst:<14}{len(seeds):>6}  ' + '  '.join(f'{fmt(row.get(c)):>7}' for c in configs)
          + f"  {fmt(a['ils']):>7}  {fmt(a['sa']):>7}  {best_cfg:>14}  {gap:+.2f}%")

print('\nMean MSILS iterations in the time limit')
print(f"{'instance':<14}      " + '  '.join(f'{c:>7}' for c in configs))
for inst in instances:
    print(f'{inst:<14}      ' + '  '.join(
        f"{mean(v[1] for v in msils[(inst, c)].values()):>7.0f}" if (inst, c) in msils else '      -' for c in configs))
