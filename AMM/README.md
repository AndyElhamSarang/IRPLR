# AMM reimplementation

C++ reimplementation of the ILS and SA of

> Alvarez, A., Munari, P., Morabito, R. (2018). Iterated local search and simulated
> annealing algorithms for the inventory routing problem. *International Transactions in
> Operational Research* 25(6), 1785–1809. doi:10.1111/itor.12547

It is a baseline for the MSILS in `../Heuristic`. AMM report logistic-ratio (OF2) results
only for the Archetti et al. (2007) instances (up to 50 customers); for the 50–200 customer
instances they report total cost only (their Table 7). A reimplementation is therefore
needed to compare on the large instances under the same hardware and time limit.

## Build and run

```
make            # ./amm  (-O2)
make debug      # ./amm_debug (address/undefined sanitizers)
./amm ../Data/H3/abs1n10_2.dat --algo ils --obj lr --time 30 --seed 1 --csv out.csv --sol sol.txt
scripts/run_batch.sh ../Data/Large ils 60 5 results/large_ils.csv
```

Options: `--algo ils|sa`, `--obj lr|tc` (OF2 / OF1), `--time` seconds, `--seed`,
`--vehicles K` (default: file-name suffix), `--maxIterRVND` (default 5), `--maxPerturb`
(default 7), `--verbose`. Every final solution is re-verified from scratch
(`Solution::check`); the exit code is 0 only if the check passes.

**Run experiments sequentially.** Parallel runs on fewer cores than processes make the
wall-clock time limit meaningless.

## Problem conventions (identical to MSILS)

- Distances: Euclidean rounded to the nearest integer (NINT).
- Customer: `I[t] = I[t-1] + q[t] - d >= 0`, `I[t-1] + q[t] <= U`.
- Supplier: `S[t] = S[t-1] + r - sum_i q[i][t] >= 0` (production of period t is available in t).
- At most one visit per customer and period; vehicle capacity per route.
- Number of vehicles from the file-name suffix (`abs1n10_3.dat` → 3); capacities in the
  data files are already divided by the number of vehicles and rounded down.
- OF1 holding cost (only used with `--obj tc`): end-of-period stock, periods 1..H.

## Mapping to the paper

| Paper | Code |
|---|---|
| s3.1, Algorithm 1 (construction) | `src/construction.cpp` — same logic as `ConstructAMMInitialSolution` in `../Heuristic/src/InitialWithZone.cpp`, so MSILS-Initial-Alt and this code build identical initial solutions |
| s3.2, Algorithm 2 (ILS) | `runILS` in `src/search.cpp` |
| s3.2.1, Algorithm 3 (RVND), multi-start RVND | `rvnd`, `multiRVND` in `src/search.cpp` |
| s3.2.1, seven operators | `src/operators.cpp` |
| s3.2.2 perturbation | `perturb` in `src/random_moves.cpp` |
| s3.3, Algorithm 4 (SA), s3.3.1 neighbours | `runSA` in `src/search.cpp`, `randomMove` in `src/random_moves.cpp` |
| Table 1 / s4.2 parameters | `Params` in `src/amm.h` (max_perturb 7, maxIterRVND 5, n_sol 10, maxI 25, Temp0 3000, Tempmin 1e-6, α 0.99) |

## Interpretation choices (where the paper is silent)

1. **Operator application in RVND.** "Apply v" = find the best improving move of the whole
   neighbourhood (best improvement) and apply it once; an improvement resets the operator
   set (Algorithm 3).
2. **Routing operators** (Or-opt(k), k∈{1,2,3}; Shift(k), k∈{1,2,3}; Swap(k1,k2),
   k1≥k2, k1,k2∈{1,2}) move customers with their current quantities; a move is rejected if
   it violates vehicle capacity. Segments are not reversed. Shift may use an empty vehicle.
3. **Increase/reduce deliveries.** Increase = largest feasible increase w.r.t. vehicle
   capacity, customer capacity in all later periods and supplier stock. OF2: increases for
   all customers; a reduction is only considered when the visit can be removed completely
   (paper). OF1: increase if h0 > hi, otherwise reduce as much as possible (the visit is
   removed if its quantity reaches zero).
4. **Merge visits.** Any two visits of a customer; the merged visit keeps the route
   position of the retained visit; vehicle capacity, customer capacity, stockout and
   supplier stock are checked.
5. **Transfer visits.** The visit keeps its quantity and is inserted at the cheapest
   feasible position over all vehicles of the target period.
6. **Insert visits.** The new visit receives the maximum feasible quantity given the
   vehicle's residual capacity; vehicle and position are those of the best objective change.
7. **Perturbation.** One operator per call, drawn at random; it perturbs a number of
   elements drawn uniformly from {1,…,max_perturb}; operators are tried in random order until
   one changes the solution. Random shift/insertion use the cheapest position of the chosen
   route; random split moves half of a delivery to a random other period (added to an existing
   visit or inserted at the cheapest feasible position), which can create non-integer
   quantities (the paper imposes no integrality).
8. **SA neighbours.** One random feasible move of a randomly chosen operator (up to 30
   draws to find a feasible one). For OF2 the increase/reduce neighbour picks increase or
   complete removal with equal probability.
9. **Multi-start RVND** restarts RVND `maxIterRVND` times from the same input solution and
   keeps the best result.
10. **Same parameters for OF1 and OF2**, as stated in the paper (they were tuned with
    ParamILS on OF1).

## Validation status

- All final solutions pass the independent feasibility check; the sanitizer build runs
  clean on H3–H6 instances for ILS/SA × OF1/OF2.
- Against the 15 MIP optima in `../MIP/Optimal_results_*.csv` (H3, 5–10 customers, 1–2
  vehicles), best of 5 seeds: ILS average gap 0.14% (optimal on 14/15). AMM Table 11 reports
  1.09% for their ILS on H3. Note: these runs were made in parallel on a single core, so the
  effective time per run was about 1 s.
- **SA is weaker than published** on these instances (1.79% vs 0.03% in AMM Table 11) and
  stops by minimum temperature after 2172 temperature levels. With Temp0 = 3000 the
  acceptance test is meaningless for OF2 (objective changes of order 0.01) until the
  temperature has dropped by five orders of magnitude, so most of the run is a random walk
  followed by RVND. This follows from the published parameters, but the published SA results
  are better, so the original code may differ in an unreported detail. Treat SA results with
  caution and report this.
- Still to do: reproduce AMM Tables 9–11 (OF2, H3/H4/H5/H6 and L sets) with their protocol
  (best of 5 runs, 30 s) to quantify how close the reimplementation is to the original.
