"""Paired comparison of MSILS local search orders (SEQUENTIAL vs RVND) from run_rvnd.sh output.

Per instance: average (and best) logistic ratio over the seeds that both variants completed.
Gap = 100 * (RVND - SEQUENTIAL) / SEQUENTIAL, so a negative gap means RVND is better.
Reports class averages by number of customers n and vehicles K, win/tie/loss counts and a
two-sided Wilcoxon signed-rank test (normal approximation with tie correction) on the
per-instance averages.

usage: python3 scripts/compare_rvnd.py [results/rvnd_test45_60s.csv]
"""
import csv, math, re, sys
from collections import defaultdict

src = sys.argv[1] if len(sys.argv) > 1 else 'results/rvnd_test45_60s.csv'
lr = defaultdict(dict)  # (inst, order) -> {seed: lr}
iters = defaultdict(list)
fails = 0
for r in csv.DictReader(open(src)):
    if r['status'] != 'ok':
        fails += 1
        continue
    lr[(r['instance'], r['order'])][int(r['seed'])] = float(r['logistic_ratio'])
    iters[r['order']].append(int(r['iterations']))

def wilcoxon(d):
    d = [x for x in d if abs(x) > 1e-12]
    n = len(d)
    if n == 0:
        return float('nan'), 0
    a = sorted(range(n), key=lambda i: abs(d[i]))
    ranks = [0.0] * n
    i = ties = 0
    while i < n:
        j = i
        while j + 1 < n and abs(abs(d[a[j + 1]]) - abs(d[a[i]])) < 1e-12:
            j += 1
        for k in range(i, j + 1):
            ranks[a[k]] = (i + j) / 2 + 1
        t = j - i + 1
        ties += t ** 3 - t
        i = j + 1
    wp = sum(r for r, x in zip(ranks, d) if x > 0)
    mu = n * (n + 1) / 4
    sd = math.sqrt(n * (n + 1) * (2 * n + 1) / 24 - ties / 48)
    z = (wp - mu) / sd if sd > 0 else 0.0
    return math.erfc(abs(z) / math.sqrt(2)), n

rows = []
for inst in sorted({i for i, _ in lr}):
    a, b = lr.get((inst, 'SEQUENTIAL'), {}), lr.get((inst, 'RVND'), {})
    seeds = sorted(set(a) & set(b))
    if not seeds:
        continue
    n, k = map(int, re.match(r'abs\d+n(\d+)_(\d+)', inst).groups())
    sa = sum(a[s] for s in seeds) / len(seeds)
    sb = sum(b[s] for s in seeds) / len(seeds)
    ba, bb = min(a[s] for s in seeds), min(b[s] for s in seeds)
    rows.append(dict(inst=inst, n=n, K=k, seeds=len(seeds), avg_seq=sa, avg_rvnd=sb,
                     gap_avg=100 * (sb - sa) / sa, gap_best=100 * (bb - ba) / ba))

if not rows:
    sys.exit('no paired results yet')
print(f'{len(rows)} instances with paired results, {sum(r["seeds"] for r in rows)} seed pairs, {fails} failed runs')
for o in ('SEQUENTIAL', 'RVND'):
    if iters[o]:
        print(f'  mean ILS iterations per run, {o}: {sum(iters[o]) / len(iters[o]):.0f}')

def summary(label, rs):
    g = [r['gap_avg'] for r in rs]
    gb = [r['gap_best'] for r in rs]
    w = sum(x < -1e-9 for x in g); l = sum(x > 1e-9 for x in g)
    print(f'{label:<10} {len(rs):>4} {sum(g) / len(g):>+9.2f} {sum(gb) / len(gb):>+9.2f}   {w:>3}/{len(g) - w - l}/{l}')

print('\nGap of RVND vs SEQUENTIAL (%), negative = RVND better')
print(f'{"class":<10} {"inst":>4} {"avg-gap":>9} {"best-gap":>9}   W/T/L (RVND)')
for n in sorted({r['n'] for r in rows}):
    summary(f'n={n}', [r for r in rows if r['n'] == n])
for k in sorted({r['K'] for r in rows}):
    summary(f'K={k}', [r for r in rows if r['K'] == k])
summary('all', rows)
p, m = wilcoxon([r['avg_rvnd'] - r['avg_seq'] for r in rows])
print(f'\nWilcoxon signed-rank on per-instance averages: n={m}, two-sided p={p:.4f}')
seedpairs = [lr[(r["inst"], "RVND")][s] - lr[(r["inst"], "SEQUENTIAL")][s] for r in rows
             for s in set(lr[(r["inst"], "RVND")]) & set(lr[(r["inst"], "SEQUENTIAL")])]
p2, m2 = wilcoxon(seedpairs)
print(f'Wilcoxon signed-rank on all seed pairs:        n={m2}, two-sided p={p2:.4f}')
