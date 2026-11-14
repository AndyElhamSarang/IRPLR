#pragma once
// Neighbourhood memory for the inter-route operators O1 (shift), O2 (swap) and O4 (swap-remove-insert).
// A "pair" is the unit an operator scans: (day, vehicle1, vehicle2) for O1/O2 and (day, vehicle) for O4.
// When a pair is scanned completely without a move that improves the solution the operator started from, the memory
// records it, and later scans skip the pair while the record is still valid. Selected with LocalSearchMemory:
//
//  NO      no memory (paper).
//  ROUTE   approximate, HGS-style (whenLastModified / whenLastTested, Vidal 2022): the record stays valid until one of
//          the pair's routes (O4: also the day's list of unvisited customers) changes or the Lagrange penalties change.
//          Cleared at the start of every local search.
//  MIDDLE  the record holds a fingerprint of the pair's routes, the loads and slack of its vehicles on that day and the
//          delivery/inventory/allocation rows and per-visit slack of every customer involved, plus the change in cost,
//          quantity, stockout and overload of every evaluated move. It stays valid while the fingerprint is unchanged
//          and every stored move is still non-improving under the current totals and penalties (re-tested in O(1)).
//          Ignores later-day loads of other vehicles and the supplier inventory, so it is still approximate.
//  EXACT   as MIDDLE, and the fingerprint also contains the later-day loads of the vehicles serving the customers and
//          the supplier inventory from the pair's day on. A skipped pair then has no improving move, so the search is
//          the same as with NO (up to floating-point rounding in the re-test).
//
// Audit mode (tuning binary, --LocalSearchMemoryAudit YES) never skips: it evaluates every pair and counts how often a
// pair that would have been skipped had an improving move (must be 0 for EXACT).

class solution;
class preprocessing;
class input;

namespace local_search_memory
{
    enum Neighbourhood
    {
        SHIFT = 0,
        SWAP = 1,
        SWAP_REMOVE_INSERT = 2
    };

    struct PairDecision
    {
        bool skip;       // the pair has no improving move and can be skipped
        bool hadFailure; // O1/O2 bookkeeping: the skipped pair contained failing evaluations (keep it in the pair set)
    };

    // Called at the start of every local search (ROUTE: clears the memory).
    void StartLocalSearch();
    // Called at the start of every operator scan (ROUTE: detects which routes changed).
    void StartScan(solution &IRPSolution);
    // Called before a pair is scanned. vehicle2 = -1 for O4, whose extra customers are the day's unvisited customers.
    PairDecision BeginPair(int neighbourhood, int day, int vehicle1, int vehicle2, const vector<int> &extraCustomers,
                           solution &IRPSolution, preprocessing &memory, input &IRPLR,
                           double PenaltyForStockOut, double PenaltyMoreThanCapacity, double objv_begin);
    // Called for every evaluated move that is not rejected: change in transportation cost, total delivery,
    // stockout and capacity violation.
    void AddMove(double deltaCost, double deltaQuantity, double deltaStockOut, double deltaOverload);
    // Called after the pair has been scanned completely.
    void EndPair(bool hadImprovingMove, bool hadFailure);

    void SetAudit(bool audit);
    long long SkippedPairs();
    long long ScannedPairs();
    long long AuditWouldSkip();   // audit: pairs that would have been skipped
    long long AuditMisses();      // audit: of those, pairs that had an improving move
}
