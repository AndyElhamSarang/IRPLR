#include "lib.h"
void solution_improvement::OperatorDisturb(input &IRPLR, solution &IncumbentSolution, solution &IRPSolution, int &DisturbanceCounter, int &MaxDisturbance)
{
    // cout << "Start Disturb Operator" << endl;
    // IncumbentSolution.print_solution(IRPLR);

    IRPSolution = IncumbentSolution; // For demonstration, we just copy the global best solution

    boost_random_mechanism RandomnessInDisturb;

    int MaxDisturbanceToApply = IRPLR.NumberOfVehicles; // This is a parameter that can be tuned. It determines the maximum number of routes that can be disturbed in one iteration. Setting it to the number of vehicles allows for a significant level of disturbance, but it can be adjusted based on the desired balance between exploration and exploitation in the search process.
    int DisturbanceStrength = 5;
    int DisturbanceToApply = 1 + (DisturbanceStrength * DisturbanceCounter) / MaxDisturbance;
    DisturbanceToApply = min(DisturbanceToApply, MaxDisturbanceToApply);
    cout << "Disturbance to apply: " << DisturbanceToApply
         << ", MaxDisturbanceToApply: " << MaxDisturbanceToApply
         << ", DisturbanceStrength: " << DisturbanceStrength
         << ", DisturbanceCounter: " << DisturbanceCounter << endl;
    int DisturbanceApplied = 0;
    while (DisturbanceApplied < DisturbanceToApply)
    {
        vector<vector<int>> CandidateRoutesToRemove;
        for (int i = 0; i < IRPSolution.Route.size(); i++) // For a day
        {
            if (IRPSolution.Route[i].size() != 0)
            {
                for (int j = 0; j < IRPSolution.Route[i].size(); j++) // For a vehicle
                {

                    if (IRPSolution.Route[i][j].size() >= 2) // Only consider routes with at least 2 customers for removal
                    {
                        vector<int> TempCandidateRoutesToRemove;
                        TempCandidateRoutesToRemove.push_back(i);
                        TempCandidateRoutesToRemove.push_back(j);

                        CandidateRoutesToRemove.push_back(TempCandidateRoutesToRemove);
                    }
                }
            }
        }
        for (int i = 0; i < CandidateRoutesToRemove.size(); i++)
        {
            cout << "Candidate route to remove: Day " << CandidateRoutesToRemove[i][0] << ", Vehicle " << CandidateRoutesToRemove[i][1] << ", Number of customers: " << IRPSolution.Route[CandidateRoutesToRemove[i][0]][CandidateRoutesToRemove[i][1]].size() << endl;
        }
        if (CandidateRoutesToRemove.size() == 0)
        {
            cout << "No candidate routes to remove. Exiting disturbance operator." << endl;
            break;
        }
        int SelectedDayVehicle = RandomnessInDisturb.random_number_generator(0, CandidateRoutesToRemove.size() - 1, generator);
        cout << "Selected route to remove: Day "
             << CandidateRoutesToRemove[SelectedDayVehicle][0] << ", Vehicle " << CandidateRoutesToRemove[SelectedDayVehicle][1] << endl;

        // Pick customers visits to remove from the selected route
        int SelectedRemovelEnd = RandomnessInDisturb.random_number_generator(2, IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(), generator);
        cout << "SelectedRemovelEnd: " << SelectedRemovelEnd << endl;
        int SelectedRemovelBegin = RandomnessInDisturb.random_number_generator(0, SelectedRemovelEnd - 2, generator);
        cout << "SelectedRemovelBegin: " << SelectedRemovelBegin << endl;
        assert(SelectedRemovelEnd - SelectedRemovelBegin > 1);
        int removeal_counter = 0;
        for (int i = SelectedRemovelBegin; i < SelectedRemovelEnd; i++)
        {

            // Update VehicleLoad
            IRPSolution.VehicleLoad[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]] -=
                IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]];

            // Update DeliveryQuantity
            IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]] -=
                IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]];

            // Update VehicleAllocation VisitOrder
            IRPSolution.VehicleAllocation[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
                                         [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.NumberOfVehicles + 1;
            IRPSolution.VisitOrder[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
                                  [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.Retailers.size() + 1;

            // Update InventoryLevel
            double ChangeInTotalQuantity = 0.0; // Initialize appropriately
            double NewStockOut = 0.0;           // Initialize appropriately
            double NewVehicleOverload = 0.0;    // Initialize appropriately

            ////////////////////////////////////////////////////
            //         Not AllowingCapacityViolation          //
            ////////////////////////////////////////////////////
            if (CandidateRoutesToRemove[SelectedDayVehicle][0] == 0)
            {
                AdjustQuantityAndInventoryLevel(
                    IRPLR.Retailers[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]].InventoryBegin,
                    CandidateRoutesToRemove[SelectedDayVehicle][0],
                    CandidateRoutesToRemove[SelectedDayVehicle][1],
                    IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
                    IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
                    IRPSolution.VehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    NewVehicleOverload,
                    IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
                    IRPLR);
            }
            else
            {
                AdjustQuantityAndInventoryLevel(
                    IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0] - 1],
                    CandidateRoutesToRemove[SelectedDayVehicle][0],
                    CandidateRoutesToRemove[SelectedDayVehicle][1],
                    IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
                    IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
                    IRPSolution.VehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    NewVehicleOverload,
                    IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
                    IRPLR);
            }
            // ////////////////////////////////////////////////////
            // //          AllowingCapacityViolation             //
            // ////////////////////////////////////////////////////

            // if (CandidateRoutesToRemove[SelectedDayVehicle][0] == 0)
            // {
            //     AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
            //         IRPLR.Retailers[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]].InventoryBegin,
            //         CandidateRoutesToRemove[SelectedDayVehicle][0],
            //         CandidateRoutesToRemove[SelectedDayVehicle][1],
            //         IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
            //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
            //         IRPSolution.VehicleLoad,
            //         IRPSolution.VehicleAllocation,
            //         ChangeInTotalQuantity,
            //         NewStockOut,
            //         NewVehicleOverload,
            //         IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
            //         IRPLR);
            // }
            // else
            // {
            //     AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
            //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0] - 1],
            //         CandidateRoutesToRemove[SelectedDayVehicle][0],
            //         CandidateRoutesToRemove[SelectedDayVehicle][1],
            //         IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
            //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
            //         IRPSolution.VehicleLoad,
            //         IRPSolution.VehicleAllocation,
            //         ChangeInTotalQuantity,
            //         NewStockOut,
            //         NewVehicleOverload,
            //         IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
            //         IRPLR);
            // }

            // Update Unallocated customers

            IRPSolution.UnallocatedCustomers[CandidateRoutesToRemove[SelectedDayVehicle][0]]
                .push_back(
                    IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]);

            removeal_counter++;
        }
        assert(removeal_counter == SelectedRemovelEnd - SelectedRemovelBegin);

        IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].erase(
            IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].begin() + SelectedRemovelBegin,
            IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].begin() + SelectedRemovelEnd);
        cout << "Number of disturbance applied: " << DisturbanceApplied << endl;
        DisturbanceApplied++;
    }
}