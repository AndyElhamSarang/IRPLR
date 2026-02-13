#include "lib.h"
void solution_improvement::OperatorDisturb(input &IRPLR, solution &GlobalBest, solution &IRPSolution, int &DisturbanceCounter, int &MaxDisturbance)
{
    // cout << "Start Disturb Operator" << endl;
    GlobalBest.print_solution(IRPLR);

    IRPSolution = GlobalBest; // For demonstration, we just copy the global best solution

    boost_random_mechanism RandomnessInDisturb;
   
    int MaxDisturbanceToApply = IRPLR.NumberOfVehicles; //This is a parameter that can be tuned. It determines the maximum number of routes that can be disturbed in one iteration. Setting it to the number of vehicles allows for a significant level of disturbance, but it can be adjusted based on the desired balance between exploration and exploitation in the search process.
    int DisturbanceStrength = 5;
    int DisturbanceToApply = 1 + (DisturbanceStrength*DisturbanceCounter)/MaxDisturbance;
    DisturbanceToApply = min(DisturbanceToApply, MaxDisturbanceToApply);
    cout<<"Disturbance to apply: "<<DisturbanceToApply
    <<", MaxDisturbanceToApply: "<<MaxDisturbanceToApply
    <<", DisturbanceStrength: "<<DisturbanceStrength
    <<", DisturbanceCounter: "<<DisturbanceCounter<<endl;
    int DisturbanceApplied = 0;
    while (DisturbanceApplied < DisturbanceToApply)
    {
        vector<vector<int>> CandidateRoutesToRemove;
        for (int i = 0; i < GlobalBest.Route.size(); i++) // For a day
        {

            if (GlobalBest.Route[i].size() != 0)
            {
                for (int j = 0; j < GlobalBest.Route[i].size(); j++) // For a vehicle
                {

                    if (GlobalBest.Route[i][j].size() != 0)
                    {
                        vector<int> TempCandidateRoutesToRemove;
                        TempCandidateRoutesToRemove.push_back(i);
                        TempCandidateRoutesToRemove.push_back(j);

                        CandidateRoutesToRemove.push_back(TempCandidateRoutesToRemove);
                    }
                }
            }
        }
        // for (int i = 0; i < CandidateRoutesToRemove.size(); i++)
        // {
        //     cout << "Candidate route to remove: Day " << CandidateRoutesToRemove[i][0] << ", Vehicle " << CandidateRoutesToRemove[i][1] << endl;
        // }

        int SelectedDayVehicle = RandomnessInDisturb.random_number_generator(0, CandidateRoutesToRemove.size() - 1, generator);
        // cout << "Selected route to remove: Day " << CandidateRoutesToRemove[SelectedDayVehicle][0] << ", Vehicle " << CandidateRoutesToRemove[SelectedDayVehicle][1] << endl;

        // Update VehicleLoad
        IRPSolution.VehicleLoad[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]] = 0;
        // Update DeliveryQuantity
        for (int i = 0; i < IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(); i++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]] -=
                IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]];
        }

        // Update VehicleAllocation VisitOrder
        for (int i = 0; i < IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(); i++)
        {
            IRPSolution.VehicleAllocation[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
                                         [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.NumberOfVehicles + 1;
            IRPSolution.VisitOrder[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
                                  [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.Retailers.size() + 1;
        }

        // Update InventoryLevel
        for (int i = 0; i < IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(); i++)
        {
            double ChangeInTotalQuantity = 0.0; // Initialize appropriately
            double NewStockOut = 0.0;           // Initialize appropriately
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
                    IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
                    IRPLR);
            }
        }
        // Update Unallocated customers
        for (int i = 0; i < IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(); i++)
        {
            IRPSolution.UnallocatedCustomers[CandidateRoutesToRemove[SelectedDayVehicle][0]]
                .push_back(
                    IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]);
        }
        IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].clear();
        cout<<"Number of disturbance applied: "<<DisturbanceApplied<<endl;
        DisturbanceApplied++;
    }
}