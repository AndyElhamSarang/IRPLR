#include "lib.h"

namespace
{
    void ThrowIfLocalSearchTimeLimitReached()
    {
        time(&total_end_time);
        double total_ls_time = difftime(total_end_time, total_start_time);
        assert(total_ls_time - AccumulatedTimeHGS >= 0);
        if (total_ls_time - AccumulatedTimeHGS >= MainAlgorithmTimeLimit - 0.01)
        {
            int time_limit_reached = static_cast<int>(total_ls_time);
            throw time_limit_reached;
        }
    }
}

void RecordSolution(solution &IRPSolution, solution &SolutionToRecord, double &time_stamp)
{
    IRPSolution = SolutionToRecord;
    IRPSolution.TotalTransportationCost = SolutionToRecord.TotalTransportationCost;
    IRPSolution.TotalDelivery = SolutionToRecord.TotalDelivery;
    IRPSolution.LogisticRatio = SolutionToRecord.LogisticRatio;
    IRPSolution.solution_time = time_stamp;
}
void RecordTimedSolutions(input &IRPLR, solution &IRPSolution, solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s, solution &IRPSolution60s, vector<solution> &TimeCheckpointSolutions, int &DisturbanceCounter, bool &RunHGSAtEnd)
{
    time(&total_end_time);
    double accum_time = difftime(total_end_time, total_start_time);

    bool checkpointDue = false;
    for (size_t checkpointIndex = 0; checkpointIndex < TimeCheckpointSolutions.size(); ++checkpointIndex)
    {
        const double checkpointTime = static_cast<double>((checkpointIndex + 1) * 30);
        if (accum_time >= checkpointTime && TimeCheckpointSolutions[checkpointIndex].solution_time < 0)
        {
            checkpointDue = true;
            break;
        }
    }

    if (checkpointDue)
    {
        IRPSolution.GetLogisticRatio(IRPLR);
        if (fabs(IRPSolution.ViolationStockOut - 0) < 0.00001)
        {
            if (fabs(IRPSolution.ViolationMoreThanCapacity - 0) < 0.00001)
            {
                if (GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
                {
                    assert(IRPSolution.ViolationStockOut >= -0.00001);
                    assert(IRPSolution.ViolationMoreThanCapacity >= -0.00001);
                    RecordSolution(GlobalBest, IRPSolution, accum_time);
                    RunHGSAtEnd = true;
                    DisturbanceCounter = 0;
                }
            }
        }

        for (size_t checkpointIndex = 0; checkpointIndex < TimeCheckpointSolutions.size(); ++checkpointIndex)
        {
            const double checkpointTime = static_cast<double>((checkpointIndex + 1) * 30);
            if (accum_time >= checkpointTime && TimeCheckpointSolutions[checkpointIndex].solution_time < 0)
            {
                cout << checkpointTime << "s time point reached." << endl;
                RecordSolution(TimeCheckpointSolutions[checkpointIndex], GlobalBest, accum_time);
                if (checkpointIndex == 0)
                {
                    whether_results_reported_30 = true;
                    RecordSolution(IRPSolution30s, GlobalBest, accum_time);
                }
                else if (checkpointIndex == 1)
                {
                    whether_results_reported_60 = true;
                    RecordSolution(IRPSolution60s, GlobalBest, accum_time);
                }
            }
        }
    }

    if (whether_results_reported_first_improvement == false)
    {
        cout << "First improvement time point reached." << endl;
        IRPSolution.GetLogisticRatio(IRPLR);
        if (fabs(IRPSolution.ViolationStockOut - 0) < 0.00001)
        {
            if(fabs(IRPSolution.ViolationMoreThanCapacity - 0) < 0.00001)
            {
                assert(IRPSolution.ViolationStockOut >= -0.00001);
                assert(IRPSolution.ViolationMoreThanCapacity >= -0.00001);
                if (GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
                {
                    RecordSolution(GlobalBest, IRPSolution, accum_time);
                    RunHGSAtEnd = true;
                    DisturbanceCounter = 0;
                }
            }
        }

        whether_results_reported_first_improvement = true;
        RecordSolution(FirstImprovementSolution, GlobalBest, accum_time);
    }
}

int solution_improvement::ImprovedLocalSearch(input &IRPLR, solution &IRPSolution, double &ScalarLagrangianRelaxation, preprocessing &memory, solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s, solution &IRPSolution60s, vector<solution> &TimeCheckpointSolutions, int &DisturbanceCounter, bool &RunHGSAtEnd)
{
    int whether_improved_via_local_search = 0;
    IRPSolution.UpdateSupplierInventory(IRPLR);
    memory.PopulatePrefixAndSuffix(IRPLR, IRPSolution);
    IRPSolution.GetLogisticRatio(IRPLR);
    memory.PopulateQuantityStructure(IRPLR, IRPSolution);
    // Initialise penalty for stockout
    int true_local = 0;
    double PenaltyForStockOut = 0;
    double PenaltyMoreThanCapacity = 0;

    double temp_global_best_logistic_ratio = GlobalBest.LogisticRatio;
    if (AllowLagrangianRelaxation == "YES")
    {
        InitialiseUpdateLagrangianMultipler(IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, GlobalBest.LogisticRatio, ScalarLagrangianRelaxation, true_local);

        // PenaltyForStockOut = 100000;      // A very large penalty for stockout to discourage any infeasible moves.
        // PenaltyMoreThanCapacity = 100000; // A very large penalty for exceeding capacity to discourage any infeasible moves.
    }
    else if (AllowLagrangianRelaxation == "NO")
    {
        PenaltyForStockOut = 100000;      // A very large penalty for stockout to discourage any infeasible moves.
        PenaltyMoreThanCapacity = 100000; // A very large penalty for exceeding capacity to discourage any infeasible moves.
    }
    bool whether_improved_via_SwapRemoveInsertPair = true;
    bool whether_improved_via_SwapTwoRoutesOnSingleDay = true;
    bool whether_improved_via_InterSwap = true;
    bool whether_improved_via_Repair = true;
    bool QuantityBalancing = false;
    int Abnormal_true_local_counter = 0;
    boost_random_mechanism NeighbourhoodRandomness;
   
    // Keeps the best feasible logistic ratio seen during this call (used by the multiplier update).
    auto TrackBestFeasible = [&]()
    {
        if (IRPSolution.ViolationStockOut < 0.00001 && IRPSolution.ViolationMoreThanCapacity < 0.00001)
        {
            if (temp_global_best_logistic_ratio - IRPSolution.LogisticRatio > 0.00001)
            {
                temp_global_best_logistic_ratio = IRPSolution.LogisticRatio;
            }
        }
    };

    // Each Run* applies one operator repeatedly (best improving move) until it no longer
    // improves, and returns true if at least one move was applied.

    //////////////////////////////////////////////////
    //                                              //
    //                 Operator:Shift (O1)          //
    //                                              //
    //////////////////////////////////////////////////
    auto RunShift = [&]() -> bool
    {
        bool improved = false;
        int whether_improved = 1;
        int counter = 0;
        int min_shift1 = 1;
        int max_shift1 = 3;
        int min_shift2 = 0;
        int max_shift2 = 0;
        QuantityBalancing = true;
        set<vector<int>> ShiftTwoRoutesOnSingleDayPair; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        for (int day = 0; day < IRPSolution.Route.size(); day++)
        {
            for (int vehicle_1 = 0; vehicle_1 < IRPSolution.Route[day].size(); vehicle_1++)
            {
                for (int vehicle_2 = vehicle_1 + 1; vehicle_2 < IRPSolution.Route[day].size(); vehicle_2++)
                {
                    ShiftTwoRoutesOnSingleDayPair.insert({day, vehicle_1, vehicle_2});
                }
            }
        }
        set<vector<int>> ShiftTwoRoutesOnSingleDayPairToReconsider; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        while (whether_improved == 1)
        {
            ThrowIfLocalSearchTimeLimitReached();
            // Re-add any pairs flagged to be reconsidered
            for (const auto &p : ShiftTwoRoutesOnSingleDayPairToReconsider)
            {
                ShiftTwoRoutesOnSingleDayPair.insert(p);
            }
            ShiftTwoRoutesOnSingleDayPairToReconsider.clear();
            if (QuantityBalancing == true)
            {
                whether_improved = OperatorSwapTwoRoutesOnSingleDay(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, memory,
                                                                    ShiftTwoRoutesOnSingleDayPair, ShiftTwoRoutesOnSingleDayPairToReconsider,
                                                                    min_shift1, max_shift1, min_shift2, max_shift2);
            }
            else
            {
                whether_improved = OperatorSwapTwoRoutesOnSingleDayWithoutQuantityBalancing(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, memory,
                                                                                            ShiftTwoRoutesOnSingleDayPair, ShiftTwoRoutesOnSingleDayPairToReconsider,
                                                                                            min_shift1, max_shift1, min_shift2, max_shift2);
            }
            if (whether_improved == 1)
            {
                IRPSolution.UpdateSupplierInventory(IRPLR);
                improved = true;
            }
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
            TrackBestFeasible();
            counter++;
        }
        cout << "Iteration applied for Operator: SwapTwoRoutesOnSingleDay:" << counter << endl;
        return improved;
    };

    //////////////////////////////////////////////////
    //                                              //
    //                 Operator:Swap (O2)           //
    //                                              //
    //////////////////////////////////////////////////
    auto RunSwap = [&]() -> bool
    {
        bool improved = false;
        int whether_improved = 1;
        int counter = 0;
        int min_swap1 = 1;
        int max_swap1 = 2;
        int min_swap2 = 1;
        int max_swap2 = 2;
        QuantityBalancing = true;
        set<vector<int>> SwapTwoRoutesOnSingleDayPair; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        for (int day = 0; day < IRPSolution.Route.size(); day++)
        {
            for (int vehicle_1 = 0; vehicle_1 < IRPSolution.Route[day].size(); vehicle_1++)
            {
                for (int vehicle_2 = vehicle_1 + 1; vehicle_2 < IRPSolution.Route[day].size(); vehicle_2++)
                {
                    SwapTwoRoutesOnSingleDayPair.insert({day, vehicle_1, vehicle_2});
                }
            }
        }
        set<vector<int>> SwapTwoRoutesOnSingleDayPairToReconsider; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        while (whether_improved == 1)
        {
            ThrowIfLocalSearchTimeLimitReached();
            // Re-add any pairs flagged to be reconsidered
            for (const auto &p : SwapTwoRoutesOnSingleDayPairToReconsider)
            {
                SwapTwoRoutesOnSingleDayPair.insert(p);
            }
            SwapTwoRoutesOnSingleDayPairToReconsider.clear();
            if (QuantityBalancing == true)
            {
                whether_improved = OperatorSwapTwoRoutesOnSingleDay(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, memory,
                                                                    SwapTwoRoutesOnSingleDayPair, SwapTwoRoutesOnSingleDayPairToReconsider,
                                                                    min_swap1, max_swap1, min_swap2, max_swap2);
            }
            else
            {
                whether_improved = OperatorSwapTwoRoutesOnSingleDayWithoutQuantityBalancing(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, memory,
                                                                                            SwapTwoRoutesOnSingleDayPair, SwapTwoRoutesOnSingleDayPairToReconsider,
                                                                                            min_swap1, max_swap1, min_swap2, max_swap2);
            }
            if (whether_improved == 1)
            {
                IRPSolution.UpdateSupplierInventory(IRPLR);
                improved = true;
            }
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
            TrackBestFeasible();
            counter++;
        }
        cout << "Iteration applied for Operator: SwapTwoRoutesOnSingleDay:" << counter << endl;
        return improved;
    };

    ////////////////////////////////////////////////
    //                                            //
    //              Operator:Transfer (O3)        //
    //                                            //
    ////////////////////////////////////////////////
    auto RunTransfer = [&]() -> bool
    {
        bool improved = false;
        int whether_improved = 1;
        int counter = 0;
        while (whether_improved == 1)
        {
            ThrowIfLocalSearchTimeLimitReached();
            vector<vector<int>> TransferDetails;
            for (int i = 0; i < IRPSolution.VehicleAllocation.size(); i++) // For customer i
            {
                vector<int> visitedDays;
                vector<int> unallocatedDays;
                for (int day = 0; day < IRPSolution.VehicleAllocation[i].size(); day++)
                {
                    const int vehicle = IRPSolution.VehicleAllocation[i][day];
                    if (vehicle < IRPLR.NumberOfVehicles + 1)
                    {
                        assert(vehicle < IRPLR.NumberOfVehicles);
                        visitedDays.push_back(day);
                    }
                    if (vehicle >= IRPLR.NumberOfVehicles)
                    {
                        unallocatedDays.push_back(day);
                    }
                }
                for (int visitedDay : visitedDays)
                {
                    for (int unallocatedDay : unallocatedDays)
                    {
                        if (unallocatedDay == visitedDay)
                        {
                            continue;
                        }
                        TransferDetails.push_back({i, visitedDay, unallocatedDay});
                    }
                }
            }
            whether_improved = OperatorTransfer(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, TransferDetails, memory);
            if (whether_improved == 1)
            {
                IRPSolution.UpdateSupplierInventory(IRPLR);
                improved = true;
            }
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
            TrackBestFeasible();
            counter++;
        }
        cout << "Iteration applied for Operator Transfer:" << counter << endl;
        return improved;
    };

    //////////////////////////////////////////////////////////////////////
    //                                                                  //
    //    Operator:SwapRemoveInsert with unallocated customer (O4)      //
    //                                                                  //
    //////////////////////////////////////////////////////////////////////
    auto RunSwapRemoveInsert = [&]() -> bool
    {
        bool improved = false;
        int min_remove_length = 0;
        int max_remove_length = 1;
        int min_insert_length = 0;
        int max_insert_length = 1;
        int whether_improved = 1;
        int counter = 0;
        set<vector<int>> SwapRemoveInsertPair; // Index 0: day, Index 1: vehicle
        for (int day = 0; day < IRPSolution.Route.size(); day++)
        {
            for (int vehicle = 0; vehicle < IRPSolution.Route[day].size(); vehicle++)
            {
                SwapRemoveInsertPair.insert({day, vehicle});
            }
        }
        while (whether_improved == 1)
        {
            ThrowIfLocalSearchTimeLimitReached();
            whether_improved = OperatorSwapRemoveInsert(IRPLR, IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, memory, SwapRemoveInsertPair, min_remove_length, max_remove_length, min_insert_length, max_insert_length);
            if (whether_improved == 1)
            {
                IRPSolution.UpdateSupplierInventory(IRPLR);
                whether_improved_via_SwapRemoveInsertPair = true;
                improved = true;
            }
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
            TrackBestFeasible();
            counter++;
        }
        cout << "Iteration applied for Operator: SwapRemoveInsert:" << counter << endl;
        return improved;
    };

    //////////////////////////////////////////////////////////////////////
    //                                                                  //
    //         Operator:Intra for route optimisation (O5)               //
    //   Applied only to routes changed by O1-O4 since its last call.   //
    //                                                                  //
    //////////////////////////////////////////////////////////////////////
    auto RunIntra = [&]() -> bool
    {
        bool improved = false;
        int counter = 0;
        for (int day = 0; day < memory.TrackSingleRouteOptimisation.size(); day++)
        {
            for (int vehicle = 0; vehicle < memory.TrackSingleRouteOptimisation[day].size(); vehicle++)
            {
                if (memory.TrackSingleRouteOptimisation[day][vehicle] == 1)
                {
                    int MinToReinsert = 1;
                    int MaxToReinsert = 3; // For more than, need to implement efficient evaluation for the move. Currently, it is naively implementation, which is time consuming.
                    int whether_improved = 1;
                    while (whether_improved == 1)
                    {
                        ThrowIfLocalSearchTimeLimitReached();
                        memory.PopulateSingleRouteSubpath(IRPLR, IRPSolution.Route[day][vehicle]);
                        // OperatorIntra compares candidate costs of this single route against CurrentTransportationCost
                        // (and overwrites it with the improved route cost), so it must receive the cost of this route,
                        // not the total transportation cost of the solution.
                        const vector<int> &IntraRoute = IRPSolution.Route[day][vehicle];
                        double RouteCostBeforeIntra = 0;
                        if (IntraRoute.size() != 0)
                        {
                            RouteCostBeforeIntra += IRPLR.Distance[0][IntraRoute[0] + 1];
                            for (int k = 0; k + 1 < (int)IntraRoute.size(); k++)
                            {
                                RouteCostBeforeIntra += IRPLR.Distance[IntraRoute[k] + 1][IntraRoute[k + 1] + 1];
                            }
                            RouteCostBeforeIntra += IRPLR.Distance[IntraRoute[IntraRoute.size() - 1] + 1][0];
                        }
                        double RouteCostAfterIntra = RouteCostBeforeIntra;
                        whether_improved = OperatorIntra(IRPLR, IRPSolution.Route[day][vehicle], day, vehicle, MinToReinsert, MaxToReinsert, memory, RouteCostAfterIntra);
                        if (whether_improved == 1)
                        {
                            improved = true;
                            assert(RouteCostAfterIntra < RouteCostBeforeIntra);
                            IRPSolution.TotalTransportationCost += RouteCostAfterIntra - RouteCostBeforeIntra;
                            if (day < (int)IRPSolution.TransportationCostPerRoute.size() && vehicle < (int)IRPSolution.TransportationCostPerRoute[day].size())
                            {
                                IRPSolution.TransportationCostPerRoute[day][vehicle] = RouteCostAfterIntra;
                            }
                        }
                        memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
                        RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
                    }
                    memory.TrackSingleRouteOptimisation[day][vehicle] = 0;
                    counter++;
                }
            }
        }
        IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
        cout << "Iteration applied for Operator: Intra:" << counter << endl;
        return improved;
    };

    auto UpdateMultiplier = [&]()
    {
        cout << "================================" << endl;
        InitialiseUpdateLagrangianMultipler(IRPSolution, PenaltyForStockOut, PenaltyMoreThanCapacity, temp_global_best_logistic_ratio, ScalarLagrangianRelaxation, true_local);
        if (true_local == 1)
        {
            cout << "Abnormal true_local counter: " << Abnormal_true_local_counter << ";";
            cout << "PenaltyForStockOut: " << PenaltyForStockOut << ";";
            cout << "PenaltyMoreThanCapacity: " << PenaltyMoreThanCapacity << ";";
            cout << "ScalarLagrangianRelaxation: " << ScalarLagrangianRelaxation << ";";
            cout << "IRPSolution.ViolationStockOut: " << IRPSolution.ViolationStockOut << ";";
            cout << "IRPSolution.ViolationMoreThanCapacity: " << IRPSolution.ViolationMoreThanCapacity << ";";
            cout << "temp_global_best_logistic_ratio: " << temp_global_best_logistic_ratio << ";";
            cout << "IRPSolution.LogisticRatio: " << IRPSolution.LogisticRatio << ";";
            cout << "IRPSolution.TotalTransportationCost: " << IRPSolution.TotalTransportationCost << ";";
            cout << "GlobalBest.TotalTransportationCost: " << GlobalBest.TotalTransportationCost << ";";
            Abnormal_true_local_counter++;
        }
    };

    try
    {
        while (true_local == 0 || (IRPSolution.ViolationStockOut > 0.00001 || IRPSolution.ViolationMoreThanCapacity > 0.00001))
        {
            if (LocalSearchOrder == "RVND")
            {
                // Randomised variable neighbourhood descent as in Alvarez et al. (2018), Algorithm 3:
                // draw an operator at random from V; if it improves, V is reset to all operators,
                // otherwise it is removed from V. One RVND = one Local_Search(s, alpha) call; the
                // multiplier is then updated and the loop repeats while s changed or is infeasible.
                const vector<int> AllOperators = {0, 1, 2, 3, 4};
                vector<int> V = AllOperators;
                bool improved_any = false;
                while (!V.empty())
                {
                    int idx = NeighbourhoodRandomness.random_number_generator(0, static_cast<int>(V.size()) - 1, generator);
                    bool improved = false;
                    switch (V[idx])
                    {
                    case 0: improved = RunShift(); break;
                    case 1: improved = RunSwap(); break;
                    case 2: improved = RunTransfer(); break;
                    case 3: improved = RunSwapRemoveInsert(); break;
                    case 4: improved = RunIntra(); break;
                    }
                    if (improved)
                    {
                        V = AllOperators;
                        improved_any = true;
                    }
                    else
                    {
                        V.erase(V.begin() + idx);
                    }
                }
                true_local = improved_any ? 0 : 1;
                UpdateMultiplier();
            }
            else
            {
                // SEQUENTIAL (original MSILS): O1 -> O2 -> O3 -> O4, multiplier update, O5.
                // As in the original code, only improvements by O3, O4 and O5 mark the
                // solution as not yet locally optimal.
                RunShift();
                RunSwap();
                true_local = 1;
                if (RunTransfer())
                {
                    true_local = 0;
                }
                if (RunSwapRemoveInsert())
                {
                    true_local = 0;
                }
                UpdateMultiplier();
                if (RunIntra())
                {
                    true_local = 0;
                }
            }
        }
    }


    catch (int time_limit_reached)
    {
        cout << "!Stop by time limit" << endl;
        RecordTimedSolutions(IRPLR, IRPSolution, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, DisturbanceCounter, RunHGSAtEnd);
    }
    return whether_improved_via_local_search;
}