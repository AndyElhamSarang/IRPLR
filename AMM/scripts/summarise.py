"""Summarise AMM batch results: per instance and algorithm, the LR of each run plus best,
average and worst; then class averages (n, K) of those three statistics.

usage: python3 scripts/summarise.py [results/large_60s.csv] [out_prefix=results/summary]
writes <out_prefix>_runs.csv (one row per instance x algorithm) and <out_prefix>_classes.csv
"""
import csv, re, sys
from collections import defaultdict

src = sys.argv[1] if len(sys.argv) > 1 else 'results/large_60s.csv'
prefix = sys.argv[2] if len(sys.argv) > 2 else 'results/summary'

runs = defaultdict(dict)  # (inst, algo) -> {seed: lr}
bad = 0
for r in csv.DictReader(open(src)):
    if r['objective'] != 'lr':
        continue
    if r['status'] != 'ok':
        bad += 1
        continue
    runs[(r['instance'], r['algo'])][int(r['seed'])] = float(r['logistic_ratio'])

def key(inst):
    a, n, k = map(int, re.match(r'abs(\d+)n(\d+)_(\d+)', inst).groups())
    return (n, k, a)

maxseed = max((s for d in runs.values() for s in d), default=0)
rows = []
for (inst, algo), d in sorted(runs.items(), key=lambda x: (key(x[0][0]), x[0][1])):
    v = list(d.values())
    n, k, _ = key(inst)
    rows.append(dict(instance=inst, n=n, K=k, algo=algo, runs=len(v),
                     **{f'run{s}': f'{d[s]:.6f}' if s in d else '' for s in range(1, maxseed + 1)},
                     best=min(v), average=sum(v) / len(v), worst=max(v)))

with open(f'{prefix}_runs.csv', 'w', newline='') as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
    w.writeheader()
    for r in rows:
        w.writerow({k: (f'{x:.6f}' if isinstance(x, float) else x) for k, x in r.items()})

cls = defaultdict(list)
for r in rows:
    cls[(r['n'], r['K'], r['algo'])].append(r)
with open(f'{prefix}_classes.csv', 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['n', 'K', 'algo', 'instances', 'avg_best', 'avg_average', 'avg_worst'])
    for (n, k, algo), rs in sorted(cls.items()):
        m = len(rs)
        w.writerow([n, k, algo, m] + [f"{sum(r[s] for r in rs) / m:.6f}" for s in ('best', 'average', 'worst')])

done = sum(len(d) for d in runs.values())
print(f'{done} runs ok, {bad} not ok, {len(runs)} instance-algorithm pairs')
print(f'wrote {prefix}_runs.csv and {prefix}_classes.csv')
