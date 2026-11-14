#include "lib.h"
#include <map>
#include <array>

namespace
{
    struct StoredMove
    {
        double deltaCost, deltaQuantity, deltaStockOut, deltaOverload;
    };
    struct PairRecord
    {
        // ROUTE
        long long testedAt = 0;
        double penaltyStockOut = 0, penaltyOverCapacity = 0;
        // MIDDLE / EXACT
        vector<double> fingerprint;
        vector<StoredMove> moves;
        bool hadFailure = false;
    };

    long long Stamp = 0;
    vector<vector<vector<int>>> KnownRoutes;  // ROUTE: [day][vehicle] route; index NumberOfVehicles = unvisited list
    vector<vector<long long>> LastModified;   // ROUTE: [day][vehicle or NumberOfVehicles]
    map<array<int, 4>, PairRecord> Records;   // key: neighbourhood, day, vehicle1, vehicle2

    bool Audit = false;
    long long Skipped = 0, Scanned = 0, AuditSkip = 0, AuditMiss = 0;

    // state of the pair being scanned
    bool Active = false, Recording = false, WouldSkip = false;
    array<int, 4> CurrentKey;
    double CurrentPenaltyStockOut = 0, CurrentPenaltyOverCapacity = 0;
    vector<double> CurrentFingerprint;
    vector<StoredMove> CurrentMoves;

    void AddCustomer(vector<double> &f, int c, int day, bool exact, solution &s, preprocessing &memory, input &IRPLR)
    {
        f.push_back(-1000.0 - c);
        f.push_back(day > 0 ? s.InventoryLevel[c][day - 1] : IRPLR.Retailers[c].InventoryBegin);
        for (int t = day; t < IRPLR.TimeHorizon; t++)
        {
            f.push_back(s.DeliveryQuantity[c][t]);
            f.push_back(s.InventoryLevel[c][t]);
            const int k = s.VehicleAllocation[c][t];
            f.push_back(k);
            f.push_back(memory.QuantityStructure[c][t]);
            if (exact)
                f.push_back((t > day && k >= 0 && k < IRPLR.NumberOfVehicles) ? s.VehicleLoad[t][k] : -1.0);
        }
    }

    // Every input the moves of the pair read (MIDDLE leaves out later-day loads and the supplier inventory).
    void BuildFingerprint(vector<double> &f, int day, int vehicle1, int vehicle2, const vector<int> &extra, bool exact,
                          solution &s, preprocessing &memory, input &IRPLR)
    {
        f.clear();
        for (int k : {vehicle1, vehicle2})
        {
            if (k < 0)
                continue;
            f.push_back(-1.0 - k);
            f.push_back(s.VehicleLoad[day][k]);
            f.push_back(memory.TotalSlackPerVehicle[day][k]);
            f.push_back(s.TransportationCostPerRoute[day][k]);
            for (int c : s.Route[day][k])
                f.push_back(c);
            for (int c : s.Route[day][k])
                AddCustomer(f, c, day, exact, s, memory, IRPLR);
        }
        f.push_back(-999.0);
        for (int c : extra)
            f.push_back(c);
        for (int c : extra)
            AddCustomer(f, c, day, exact, s, memory, IRPLR);
        f.push_back(UseVehicleSlack ? 1 : 0);
        if (exact)
            for (int t = day; t < IRPLR.TimeHorizon; t++)
                f.push_back(s.InventoryLevelSupplier[t]);
    }

    // True if no stored move improves on objv_begin under the current totals and penalties
    // (same arithmetic as the operators: logistic ratio + penalty * stockout + penalty * overload).
    bool StoredMovesStillNonImproving(const PairRecord &r, solution &s, double PenaltyForStockOut, double PenaltyMoreThanCapacity, double objv_begin)
    {
        for (const StoredMove &m : r.moves)
        {
            const double NewLogisticRatio = (s.TotalTransportationCost + m.deltaCost) / (s.TotalDelivery + m.deltaQuantity);
            const double temp = NewLogisticRatio + PenaltyForStockOut * (s.ViolationStockOut + m.deltaStockOut) +
                                PenaltyMoreThanCapacity * (s.ViolationMoreThanCapacity + m.deltaOverload);
            if (objv_begin - temp > 0.00001)
                return false;
        }
        return true;
    }
}

namespace local_search_memory
{
    void StartLocalSearch()
    {
        if (LocalSearchMemory == "ROUTE")
        {
            KnownRoutes.clear();
            LastModified.clear();
            Records.clear();
            Stamp++;
        }
    }

    void StartScan(solution &IRPSolution)
    {
        if (LocalSearchMemory != "ROUTE")
            return;
        const int days = IRPSolution.Route.size();
        if ((int)KnownRoutes.size() != days)
        {
            KnownRoutes.assign(days, {});
            LastModified.assign(days, {});
        }
        for (int day = 0; day < days; day++)
        {
            const int vehicles = IRPSolution.Route[day].size();
            if ((int)KnownRoutes[day].size() != vehicles + 1)
            {
                KnownRoutes[day].assign(vehicles + 1, vector<int>{-1}); // first sync marks everything modified
                LastModified[day].assign(vehicles + 1, 0);
            }
            for (int k = 0; k <= vehicles; k++)
            {
                const vector<int> &current = (k < vehicles) ? IRPSolution.Route[day][k] : IRPSolution.UnallocatedCustomers[day];
                if (current != KnownRoutes[day][k])
                {
                    KnownRoutes[day][k] = current;
                    LastModified[day][k] = ++Stamp;
                }
            }
        }
    }

    PairDecision BeginPair(int neighbourhood, int day, int vehicle1, int vehicle2, const vector<int> &extraCustomers,
                           solution &IRPSolution, preprocessing &memory, input &IRPLR,
                           double PenaltyForStockOut, double PenaltyMoreThanCapacity, double objv_begin)
    {
        Active = true;
        Recording = false;
        CurrentKey = {neighbourhood, day, vehicle1, vehicle2};
        CurrentPenaltyStockOut = PenaltyForStockOut;
        CurrentPenaltyOverCapacity = PenaltyMoreThanCapacity;
        CurrentMoves.clear();

        PairDecision decision = {false, false};
        auto found = Records.find(CurrentKey);
        if (LocalSearchMemory == "ROUTE")
        {
            if (found != Records.end() && found->second.penaltyStockOut == PenaltyForStockOut &&
                found->second.penaltyOverCapacity == PenaltyMoreThanCapacity && day < (int)LastModified.size())
            {
                const vector<long long> &modified = LastModified[day];
                const int other = (vehicle2 >= 0) ? vehicle2 : (int)modified.size() - 1; // O4: unvisited list
                decision.skip = max(modified[vehicle1], modified[other]) < found->second.testedAt;
                decision.hadFailure = true; // ROUTE keeps skipped pairs in the O1/O2 pair set
            }
        }
        else // MIDDLE or EXACT
        {
            Recording = true;
            BuildFingerprint(CurrentFingerprint, day, vehicle1, vehicle2, extraCustomers, LocalSearchMemory == "EXACT",
                             IRPSolution, memory, IRPLR);
            if (found != Records.end() && found->second.fingerprint == CurrentFingerprint &&
                StoredMovesStillNonImproving(found->second, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, objv_begin))
            {
                decision.skip = true;
                decision.hadFailure = found->second.hadFailure;
            }
        }

        WouldSkip = decision.skip;
        if (Audit && decision.skip)
        {
            AuditSkip++;
            decision.skip = false; // evaluate anyway; EndPair checks whether it had an improving move
        }
        if (decision.skip)
        {
            Skipped++;
            Active = false;
        }
        else
        {
            Scanned++;
        }
        return decision;
    }

    void AddMove(double deltaCost, double deltaQuantity, double deltaStockOut, double deltaOverload)
    {
        if (Active && Recording)
            CurrentMoves.push_back({deltaCost, deltaQuantity, deltaStockOut, deltaOverload});
    }

    void EndPair(bool hadImprovingMove, bool hadFailure)
    {
        if (!Active)
            return;
        Active = false;
        if (Audit && WouldSkip && hadImprovingMove)
            AuditMiss++;
        if (hadImprovingMove)
        {
            Records.erase(CurrentKey); // must be scanned again: its improving move may not be the one applied
            return;
        }
        PairRecord &r = Records[CurrentKey];
        if (LocalSearchMemory == "ROUTE")
        {
            r.testedAt = ++Stamp;
            r.penaltyStockOut = CurrentPenaltyStockOut;
            r.penaltyOverCapacity = CurrentPenaltyOverCapacity;
        }
        else
        {
            r.fingerprint.swap(CurrentFingerprint);
            r.moves.swap(CurrentMoves);
            r.hadFailure = hadFailure;
        }
    }

    void SetAudit(bool audit) { Audit = audit; }
    long long SkippedPairs() { return Skipped; }
    long long ScannedPairs() { return Scanned; }
    long long AuditWouldSkip() { return AuditSkip; }
    long long AuditMisses() { return AuditMiss; }
}
