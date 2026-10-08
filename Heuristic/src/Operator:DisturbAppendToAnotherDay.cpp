#include "lib.h"

void solution_improvement::OperatorDisturbAppendToAnotherDay(
    input &IRPLR,
    solution &IncumbentSolution,
    solution &IRPSolution,
    int &DisturbanceCounter,
    int &MaxDisturbance)
{
    IRPSolution = IncumbentSolution;

    if (MaxDisturbance <= 0 || IRPLR.NumberOfVehicles <= 0)
    {
        return;
    }

    boost_random_mechanism RandomnessInDisturb;
    const int disturbance_strength = 5;
    const int disturbance_limit = IRPLR.NumberOfVehicles;
    const int disturbances_to_apply = min(
        1 + (disturbance_strength * DisturbanceCounter) / MaxDisturbance,
        disturbance_limit);

    struct DisturbanceMove
    {
        int source_day;
        int source_vehicle;
        int begin;
        int end;
        int destination_day;
    };

    for (int applied = 0; applied < disturbances_to_apply; ++applied)
    {
        vector<DisturbanceMove> possible_moves;

        for (int source_day = 0; source_day < (int)IRPSolution.Route.size(); ++source_day)
        {
            for (int source_vehicle = 0;
                 source_vehicle < (int)IRPSolution.Route[source_day].size();
                 ++source_vehicle)
            {
                const vector<int> &source_route = IRPSolution.Route[source_day][source_vehicle];
                for (int begin = 0; begin + 1 < (int)source_route.size(); ++begin)
                {
                    for (int end = begin + 2; end <= (int)source_route.size(); ++end)
                    {
                        for (int destination_day = 0;
                             destination_day < (int)IRPSolution.Route.size();
                             ++destination_day)
                        {
                            if (destination_day == source_day)
                            {
                                continue;
                            }

                            bool has_duplicate_visit = false;
                            bool has_delivery_room = true;
                            for (int position = begin; position < end; ++position)
                            {
                                const int retailer = source_route[position];
                                for (int earlier_position = begin;
                                     earlier_position < position;
                                     ++earlier_position)
                                {
                                    if (source_route[earlier_position] == retailer)
                                    {
                                        has_duplicate_visit = true;
                                        break;
                                    }
                                }
                                if (has_duplicate_visit)
                                {
                                    break;
                                }

                                const double previous_inventory = destination_day == 0
                                    ? IRPLR.Retailers[retailer].InventoryBegin
                                    : IRPSolution.InventoryLevel[retailer][destination_day - 1];

                                if (IRPLR.Retailers[retailer].InventoryMax - previous_inventory <= 0.00001)
                                {
                                    has_delivery_room = false;
                                    break;
                                }

                                for (int vehicle = 0;
                                     vehicle < (int)IRPSolution.Route[destination_day].size();
                                     ++vehicle)
                                {
                                    const vector<int> &destination_route = IRPSolution.Route[destination_day][vehicle];
                                    if (find(destination_route.begin(), destination_route.end(), retailer) != destination_route.end())
                                    {
                                        has_duplicate_visit = true;
                                        break;
                                    }
                                }
                                if (has_duplicate_visit)
                                {
                                    break;
                                }
                            }

                            if (!has_duplicate_visit && has_delivery_room)
                            {
                                possible_moves.push_back(
                                    {source_day, source_vehicle, begin, end, destination_day});
                            }
                        }
                    }
                }
            }
        }

        if (possible_moves.empty())
        {
            cout << "No valid subsequence can be appended to another day without duplicate retailer visits." << endl;
            break;
        }

        const int selected_move_index = RandomnessInDisturb.random_number_generator(
            0, (int)possible_moves.size() - 1, generator);
        DisturbanceMove move = possible_moves[selected_move_index];
        int destination_vehicle = RandomnessInDisturb.random_number_generator(
            0, IRPLR.NumberOfVehicles - 1, generator);

        vector<int> moved_retailers(
            IRPSolution.Route[move.source_day][move.source_vehicle].begin() + move.begin,
            IRPSolution.Route[move.source_day][move.source_vehicle].begin() + move.end);

        cout << "Appending " << moved_retailers.size() << " retailer(s) from day "
             << move.source_day << ", vehicle " << move.source_vehicle << " to day "
             << move.destination_day << ", vehicle " << destination_vehicle << endl;

        for (int retailer : moved_retailers)
        {
            const double old_delivery = IRPSolution.DeliveryQuantity[retailer][move.source_day];
            IRPSolution.VehicleLoad[move.source_day][move.source_vehicle] -= old_delivery;
            IRPSolution.DeliveryQuantity[retailer][move.source_day] = 0.0;
            IRPSolution.VehicleAllocation[retailer][move.source_day] = IRPLR.NumberOfVehicles + 1;
            IRPSolution.VisitOrder[retailer][move.source_day] = IRPLR.Retailers.size() + 1;

            double change_in_total_quantity = -old_delivery;
            double new_stock_out = 0.0;
            double new_vehicle_overload = 0.0;
            double previous_source_inventory = move.source_day == 0
                ? IRPLR.Retailers[retailer].InventoryBegin
                : IRPSolution.InventoryLevel[retailer][move.source_day - 1];

            AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
                previous_source_inventory,
                move.source_day,
                move.source_vehicle,
                IRPSolution.DeliveryQuantity[retailer],
                IRPSolution.InventoryLevel[retailer],
                IRPSolution.VehicleLoad,
                IRPSolution.VehicleAllocation,
                change_in_total_quantity,
                new_stock_out,
                new_vehicle_overload,
                retailer,
                IRPLR);

            double previous_destination_inventory = move.destination_day == 0
                ? IRPLR.Retailers[retailer].InventoryBegin
                : IRPSolution.InventoryLevel[retailer][move.destination_day - 1];
            const double destination_delivery = max(
                0.0,
                IRPLR.Retailers[retailer].InventoryMax - previous_destination_inventory);

            IRPSolution.DeliveryQuantity[retailer][move.destination_day] = destination_delivery;
            IRPSolution.VehicleAllocation[retailer][move.destination_day] = destination_vehicle;
            IRPSolution.VehicleLoad[move.destination_day][destination_vehicle] += destination_delivery;
            change_in_total_quantity = destination_delivery;

            AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
                previous_destination_inventory,
                move.destination_day,
                destination_vehicle,
                IRPSolution.DeliveryQuantity[retailer],
                IRPSolution.InventoryLevel[retailer],
                IRPSolution.VehicleLoad,
                IRPSolution.VehicleAllocation,
                change_in_total_quantity,
                new_stock_out,
                new_vehicle_overload,
                retailer,
                IRPLR);
        }

        vector<int> &source_route = IRPSolution.Route[move.source_day][move.source_vehicle];
        source_route.erase(source_route.begin() + move.begin, source_route.begin() + move.end);
        vector<int> &destination_route = IRPSolution.Route[move.destination_day][destination_vehicle];
        destination_route.insert(destination_route.end(), moved_retailers.begin(), moved_retailers.end());

        for (int retailer : moved_retailers)
        {
            vector<int> &source_unallocated = IRPSolution.UnallocatedCustomers[move.source_day];
            if (find(source_unallocated.begin(), source_unallocated.end(), retailer) == source_unallocated.end())
            {
                source_unallocated.push_back(retailer);
            }

            vector<int> &destination_unallocated = IRPSolution.UnallocatedCustomers[move.destination_day];
            destination_unallocated.erase(
                remove(destination_unallocated.begin(), destination_unallocated.end(), retailer),
                destination_unallocated.end());
        }

        for (int position = 0; position < (int)source_route.size(); ++position)
        {
            IRPSolution.VisitOrder[source_route[position]][move.source_day] = position;
        }
        for (int position = 0; position < (int)destination_route.size(); ++position)
        {
            IRPSolution.VisitOrder[destination_route[position]][move.destination_day] = position;
        }
    }
}





// #include "lib.h"
// void solution_improvement::OperatorDisturbAppendToAnotherDay(input &IRPLR, solution &IncumbentSolution, solution &IRPSolution, int &DisturbanceCounter, int &MaxDisturbance)
// {
//     cout << "Start OperatorDisturbAppendToAnotherDay" << endl;
//     // IncumbentSolution.print_solution(IRPLR);

//     IRPSolution = IncumbentSolution; // For demonstration, we just copy the global best solution

//     boost_random_mechanism RandomnessInDisturb;

//     int MaxDisturbanceToApply = IRPLR.NumberOfVehicles; // This is a parameter that can be tuned. It determines the maximum number of routes that can be disturbed in one iteration. Setting it to the number of vehicles allows for a significant level of disturbance, but it can be adjusted based on the desired balance between exploration and exploitation in the search process.
//     int DisturbanceStrength = 5;
//     int DisturbanceToApply = 1 + (DisturbanceStrength * DisturbanceCounter) / MaxDisturbance;
//     DisturbanceToApply = min(DisturbanceToApply, MaxDisturbanceToApply);
//     cout << "Disturbance to apply: " << DisturbanceToApply
//          << ", MaxDisturbanceToApply: " << MaxDisturbanceToApply
//          << ", DisturbanceStrength: " << DisturbanceStrength
//          << ", DisturbanceCounter: " << DisturbanceCounter << endl;
//     int DisturbanceApplied = 0;
//     while (DisturbanceApplied < DisturbanceToApply)
//     {
//         vector<vector<int>> CandidateRoutesToRemove;
//         for (int i = 0; i < IRPSolution.Route.size(); i++) // For a day
//         {
//             if (IRPSolution.Route[i].size() != 0)
//             {
//                 for (int j = 0; j < IRPSolution.Route[i].size(); j++) // For a vehicle
//                 {

//                     if (IRPSolution.Route[i][j].size() >= 2) // Only consider routes with at least 2 customers for removal
//                     {
//                         vector<int> TempCandidateRoutesToRemove;
//                         TempCandidateRoutesToRemove.push_back(i);
//                         TempCandidateRoutesToRemove.push_back(j);

//                         CandidateRoutesToRemove.push_back(TempCandidateRoutesToRemove);
//                     }
//                 }
//             }
//         }
//         for (int i = 0; i < CandidateRoutesToRemove.size(); i++)
//         {
//             cout << "Candidate route to remove: Day " << CandidateRoutesToRemove[i][0] << ", Vehicle " << CandidateRoutesToRemove[i][1] << ", Number of customers: " << IRPSolution.Route[CandidateRoutesToRemove[i][0]][CandidateRoutesToRemove[i][1]].size() << endl;
//         }
//         if (CandidateRoutesToRemove.size() == 0)
//         {
//             cout << "No candidate routes to remove. Exiting disturbance operator." << endl;
//             break;
//         }
//         int SelectedDayVehicle = RandomnessInDisturb.random_number_generator(0, CandidateRoutesToRemove.size() - 1, generator);
//         cout << "Selected route to remove: Day "
//              << CandidateRoutesToRemove[SelectedDayVehicle][0] << ", Vehicle " << CandidateRoutesToRemove[SelectedDayVehicle][1] << endl;

//         // Pick customers visits to remove from the selected route
//         int SelectedRemovelEnd = RandomnessInDisturb.random_number_generator(2, IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].size(), generator);
//         cout << "SelectedRemovelEnd: " << SelectedRemovelEnd << endl;
//         int SelectedRemovelBegin = RandomnessInDisturb.random_number_generator(0, SelectedRemovelEnd - 2, generator);
//         cout << "SelectedRemovelBegin: " << SelectedRemovelBegin << endl;
//         assert(SelectedRemovelEnd - SelectedRemovelBegin > 1);

        
//         int removeal_counter = 0;
//         for (int i = SelectedRemovelBegin; i < SelectedRemovelEnd; i++)
//         {

//             // Update VehicleLoad
//             IRPSolution.VehicleLoad[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]] -=
//                 IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]];

//             // Update DeliveryQuantity
//             IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]] -=
//                 IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0]];

//             // Update VehicleAllocation VisitOrder
//             IRPSolution.VehicleAllocation[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
//                                          [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.NumberOfVehicles + 1;
//             IRPSolution.VisitOrder[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]]
//                                   [CandidateRoutesToRemove[SelectedDayVehicle][0]] = IRPLR.Retailers.size() + 1;

//             // Update InventoryLevel
//             double ChangeInTotalQuantity = 0.0; // Initialize appropriately
//             double NewStockOut = 0.0;           // Initialize appropriately
//             double NewVehicleOverload = 0.0;    // Initialize appropriately

//             // ////////////////////////////////////////////////////
//             // //         Not AllowingCapacityViolation          //
//             // ////////////////////////////////////////////////////
//             // if (CandidateRoutesToRemove[SelectedDayVehicle][0] == 0)
//             // {
//             //     AdjustQuantityAndInventoryLevel(
//             //         IRPLR.Retailers[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]].InventoryBegin,
//             //         CandidateRoutesToRemove[SelectedDayVehicle][0],
//             //         CandidateRoutesToRemove[SelectedDayVehicle][1],
//             //         IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//             //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//             //         IRPSolution.VehicleLoad,
//             //         IRPSolution.VehicleAllocation,
//             //         ChangeInTotalQuantity,
//             //         NewStockOut,
//             //         NewVehicleOverload,
//             //         IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
//             //         IRPLR);
//             // }
//             // else
//             // {
//             //     AdjustQuantityAndInventoryLevel(
//             //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0] - 1],
//             //         CandidateRoutesToRemove[SelectedDayVehicle][0],
//             //         CandidateRoutesToRemove[SelectedDayVehicle][1],
//             //         IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//             //         IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//             //         IRPSolution.VehicleLoad,
//             //         IRPSolution.VehicleAllocation,
//             //         ChangeInTotalQuantity,
//             //         NewStockOut,
//             //         NewVehicleOverload,
//             //         IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
//             //         IRPLR);
//             // }
//             ////////////////////////////////////////////////////
//             //          AllowingCapacityViolation             //
//             ////////////////////////////////////////////////////

//             if (CandidateRoutesToRemove[SelectedDayVehicle][0] == 0)
//             {
//                 AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
//                     IRPLR.Retailers[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]].InventoryBegin,
//                     CandidateRoutesToRemove[SelectedDayVehicle][0],
//                     CandidateRoutesToRemove[SelectedDayVehicle][1],
//                     IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//                     IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//                     IRPSolution.VehicleLoad,
//                     IRPSolution.VehicleAllocation,
//                     ChangeInTotalQuantity,
//                     NewStockOut,
//                     NewVehicleOverload,
//                     IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
//                     IRPLR);
//             }
//             else
//             {
//                 AdjustQuantityAndInventoryLevelAllowingCapacityViolation(
//                     IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]][CandidateRoutesToRemove[SelectedDayVehicle][0] - 1],
//                     CandidateRoutesToRemove[SelectedDayVehicle][0],
//                     CandidateRoutesToRemove[SelectedDayVehicle][1],
//                     IRPSolution.DeliveryQuantity[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//                     IRPSolution.InventoryLevel[IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]],
//                     IRPSolution.VehicleLoad,
//                     IRPSolution.VehicleAllocation,
//                     ChangeInTotalQuantity,
//                     NewStockOut,
//                     NewVehicleOverload,
//                     IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i],
//                     IRPLR);
//             }

//             // Update Unallocated customers

//             IRPSolution.UnallocatedCustomers[CandidateRoutesToRemove[SelectedDayVehicle][0]]
//                 .push_back(
//                     IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]][i]);

//             removeal_counter++;
//         }
//         assert(removeal_counter == SelectedRemovelEnd - SelectedRemovelBegin);

//         IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].erase(
//             IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].begin() + SelectedRemovelBegin,
//             IRPSolution.Route[CandidateRoutesToRemove[SelectedDayVehicle][0]][CandidateRoutesToRemove[SelectedDayVehicle][1]].begin() + SelectedRemovelEnd);
//         cout << "Number of disturbance applied: " << DisturbanceApplied << endl;
//         DisturbanceApplied++;
//         IRPSolution.print_solution(IRPLR);
//     }
// }