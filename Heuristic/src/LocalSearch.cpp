#include "lib.h"
int solution_improvement::LocalSearch(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory)
{
    int whether_improved_via_local_search = 0;
    memory.PopulatePrefixAndSuffix(IRPLR, IRPSolution);

    bool whether_improved_via_SwapRemoveInsertPair = true;
    bool whether_improved_via_SwapTwoRoutesOnSingleDay = true;
    bool whether_improved_via_InterSwap = true;
    int true_local = 0;
    while (true_local == 0)
    {
        true_local = 1;

        int whether_improved= 1;
        int counter = 0;
        int min_swap1 = 0;
        int max_swap1 = 2;
        int min_swap2 = 0;
        int max_swap2 = 2;


        set<vector<int>> SwapTwoRoutesOnSingleDayPair; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        for(int day=0; day<IRPSolution.Route.size(); day++)
        {
            for(int vehicle_1=0; vehicle_1<IRPSolution.Route[day].size(); vehicle_1++)
            {
                for(int vehicle_2=vehicle_1+1; vehicle_2<IRPSolution.Route[day].size(); vehicle_2++)
                {
                    if(vehicle_2 > vehicle_1)
                    {
                        vector<int> temp_pair;
                        temp_pair.push_back(day);
                        temp_pair.push_back(vehicle_1);
                        temp_pair.push_back(vehicle_2);
                        SwapTwoRoutesOnSingleDayPair.insert(temp_pair);
                    }
                }
            }
        }
        int InitialSizeOfSwapTwoRoutesOnSingleDayPair = SwapTwoRoutesOnSingleDayPair.size();
        set<vector<int>> SwapTwoRoutesOnSingleDayPairToReconsider; // Index 0: day, Index 1: vehicle_1, Index 2: vehicle_2
        while (whether_improved == 1)
        {
            // Re-add any pairs flagged to be reconsidered
            for (const auto &p : SwapTwoRoutesOnSingleDayPairToReconsider)
            {
                SwapTwoRoutesOnSingleDayPair.insert(p);
            }
            SwapTwoRoutesOnSingleDayPairToReconsider.clear();
            // cout << "InitialSizeOfSwapTwoRoutesOnSingleDayPair: " << InitialSizeOfSwapTwoRoutesOnSingleDayPair <<", SwapTwoRoutesOnSingleDayPairToReconsider: "<< SwapTwoRoutesOnSingleDayPairToReconsider.size()<<", Size of reduced SwapTwoRoutesOnSingleDayPair: " << SwapTwoRoutesOnSingleDayPair.size() << endl;
            whether_improved= OperatorSwapTwoRoutesOnSingleDay(
                IRPLR,
                IRPSolution,
                PenaltyForStockOut,
                memory,
                SwapTwoRoutesOnSingleDayPair,
                SwapTwoRoutesOnSingleDayPairToReconsider,
                min_swap1,
                max_swap1,
                min_swap2,
                max_swap2
            );
            if (whether_improved == 1)
            {
                true_local = 0;
            }
            // IRPSolution.Validation(IRPLR);
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            counter++;
        }

        cout << "Iteration applied for Operator: SwapTwoRoutesOnSingleDay:" << counter << endl;

        int min_remove_length = 0;
        int max_remove_length = 2;
        int min_insert_length = 0;
        int max_insert_length = 2;
        whether_improved = 1;
        counter = 0;
        set<vector<int>> SwapRemoveInsertPair; // Index 0: day, Index 1: vehicle
        for (int day = 0; day < IRPSolution.Route.size(); day++)
        {
            for (int vehicle = 0; vehicle < IRPSolution.Route[day].size(); vehicle++)
            {
                vector<int> temp_pair;
                temp_pair.push_back(day);
                temp_pair.push_back(vehicle);
                SwapRemoveInsertPair.insert(temp_pair);
            }
        }

        while (whether_improved == 1)
        {

            whether_improved = OperatorSwapRemoveInsert(IRPLR, IRPSolution, PenaltyForStockOut, memory, SwapRemoveInsertPair, min_remove_length, max_remove_length, min_insert_length, max_insert_length);
            if (whether_improved == 1)
            {
                whether_improved_via_SwapRemoveInsertPair = true;
                true_local = 0;
            }
            memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
            // IRPSolution.Validation(IRPLR);
            counter++;
        }
        cout << "Iteration applied for Operator: SwapRemoveInsert:" << counter << endl;

        counter = 0;
        for (int day = 0; day < memory.TrackSingleRouteOptimisation.size(); day++)
        {
            for (int vehicle = 0; vehicle < memory.TrackSingleRouteOptimisation[day].size(); vehicle++)
            {
                if (memory.TrackSingleRouteOptimisation[day][vehicle] == 1)
                {
                    int MinToReinsert = 1;
                    int MaxToReinsert = 2;
                    whether_improved = 1;
                    while (whether_improved == 1)
                    {
                        memory.PopulateSingleRouteSubpath(IRPLR, IRPSolution.Route[day][vehicle]);
                        whether_improved = OperatorIntra(IRPLR, IRPSolution.Route[day][vehicle], day, vehicle, MinToReinsert, MaxToReinsert, memory, IRPSolution.TotalTransportationCost);
                        if (whether_improved == 1)
                        {
                            true_local = 0;
                        }
                        memory.UpdatePrefixAndSuffix(IRPLR, IRPSolution);
                    }
                    memory.TrackSingleRouteOptimisation[day][vehicle] = 0;
                    counter++;
                }
            }
        }
        cout << "Iteration applied for Operator: Intra:" << counter << endl;
    }
    return whether_improved_via_local_search;
}