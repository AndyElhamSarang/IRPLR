#include "lib.h"

int solution_improvement::OperatorSwapTwoRoutesOnSingleDayWithoutQuantityBalancing(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, double &PenaltyMoreThanCapacity, preprocessing &memory, set<vector<int>> &SwapTwoRoutesOnSingleDayPair, set<vector<int>> &SwapTwoRoutesOnSingleDayPairToReconsider,
                                                                                   int &min_Swap_length1, int &max_Swap_length1,
                                                                                   int &min_Swap_length2, int &max_Swap_length2)
{
    cout << "================================" << endl;
    cout << "OperatorSwapTwoRoutesOnSingleDayWithoutQuantityBalancing" << endl;
    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, IRPSolution.ViolationMoreThanCapacity);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut: " << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut: " << PenaltyForStockOut << "\t ViolationMoreThanCapacity: " << IRPSolution.ViolationMoreThanCapacity << "\t PenaltyMoreThanCapacity: " << PenaltyMoreThanCapacity << "\t LR_objv:" << LR_objv << endl;

    // IRPSolution.print_solution(IRPLR);
    // for (int i = 0; i < IRPSolution.TransportationCostPerRoute.size(); i++)
    // {
    //     for (int j = 0; j < IRPSolution.TransportationCostPerRoute[i].size(); j++)
    //     {
    //         cout << "TransportationCostPerRoute[" << i << "][" << j << "]:" << IRPSolution.TransportationCostPerRoute[i][j] << ",";
    //     }
    //     cout << endl;
    // }
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, IRPSolution.ViolationMoreThanCapacity);
    double objv_begin = LR_objv;
    // cout << "IRPSolution.TotalTransportationCost:" << IRPSolution.TotalTransportationCost << ", IRPSolution.TotalDelivery:" << IRPSolution.TotalDelivery << ", LR objv:" << LR_objv << endl;
    vector<int> move;
    for (int i = 0; i < 9; i++)
    {
        move.push_back(0);
        // move[0]: pick_day
        // move[1]: pick_vehicle1
        // move[2]: pick_position_in_vehicle1
        // move[3]: pick_vehicle2
        // move[4]: pick_position_in_vehicle2
        // move[5]: Swap_length1
        // move[6]: Swap_length2
        // move[7]: VehicleLoad1
        // move[8]: VehicleLoad2
    }
    int whether_improved_or_not = 0;

    double ImpVehicleOverload = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;
    int solutionCounter = 0;
    int working_solutionCounter = 0;
    int naive_implementation = 0;
    set<vector<int>>::iterator select_pair;
    time(&accumulate_start_time);
    try
    {
        for (select_pair = SwapTwoRoutesOnSingleDayPair.begin(); select_pair != SwapTwoRoutesOnSingleDayPair.end(); /*select_pair++*/)
        {
            const vector<int> &pair = (*select_pair);
            int pick_day = pair[0];
            int pick_vehicle1 = pair[1];
            int pick_vehicle2 = pair[2];
            int pair_obtain_improving = 0;

            assert(pick_vehicle2 > pick_vehicle1);

            for (int pick_position_in_vehicle1 = 0; pick_position_in_vehicle1 <= IRPSolution.Route[pick_day][pick_vehicle1].size(); pick_position_in_vehicle1++) // A position in vehicle 1
            {
                for (int pick_position_in_vehicle2 = 0; pick_position_in_vehicle2 <= IRPSolution.Route[pick_day][pick_vehicle2].size(); pick_position_in_vehicle2++) // A position in vehicle 1
                {
                    for (int Swap_length1 = min_Swap_length1; Swap_length1 <= max_Swap_length1; Swap_length1++)
                    {
                        for (int Swap_length2 = min_Swap_length2; Swap_length2 <= max_Swap_length2; Swap_length2++)
                        {
                            if (Swap_length1 != 0 || Swap_length2 != 0)
                            {

                                if (pick_position_in_vehicle2 + Swap_length2 <= IRPSolution.Route[pick_day][pick_vehicle2].size() && pick_position_in_vehicle1 + Swap_length1 <= IRPSolution.Route[pick_day][pick_vehicle1].size()) // Make sure the operator does not go outside the range
                                {
                                    time(&total_end_time);
                                    double total_ls_time = difftime(total_end_time, total_start_time);
                                    assert(total_ls_time - AccumulatedTimeHGS >= 0);
                                    if (total_ls_time - MainAlgorithmTimeLimit - AccumulatedTimeHGS > -0.01)
                                    {
                                        int time_limit_reached = total_ls_time;
                                        throw time_limit_reached;
                                    }
                                    if (Swap_length1 <= 3 && Swap_length2 <= 3)
                                    {

                                        ////////////////////////////////////////////////////////////////////////////////
                                        //                                                                            //
                                        //      Efficient implementation of evaluating the swap remove insert move    //
                                        //                                                                            //
                                        ////////////////////////////////////////////////////////////////////////////////
                                        solutionCounter++;
                                        cout << "pick_day:" << pick_day << ",pick_vehicle1:" << pick_vehicle1 << ",pick_position_in_vehicle1:" << pick_position_in_vehicle1 << ",Swap_length1:" << Swap_length1 << ",pick_vehicle2:" << pick_vehicle2 << ",pick_position_in_vehicle2:" << pick_position_in_vehicle2 << ",Swap_length2:" << Swap_length2 << endl;
                                        double temp_load_vehicle1 = IRPSolution.VehicleLoad[pick_day][pick_vehicle1];
                                        double temp_load_vehicle2 = IRPSolution.VehicleLoad[pick_day][pick_vehicle2];
                                        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
                                        {
                                            temp_load_vehicle1 = temp_load_vehicle1 - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day];
                                            temp_load_vehicle2 = temp_load_vehicle2 + IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day];
                                        }
                                        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
                                        {
                                            temp_load_vehicle2 = temp_load_vehicle2 - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day];
                                            temp_load_vehicle1 = temp_load_vehicle1 + IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day];
                                        }

                                        working_solutionCounter++;
                                        double NewRoute1Cost = memory.ConcatenateSwapTwoRoutesSingleDay(
                                            pick_day,
                                            pick_vehicle1,
                                            pick_position_in_vehicle1,
                                            Swap_length1,
                                            pick_vehicle2,
                                            pick_position_in_vehicle2,
                                            Swap_length2,
                                            IRPSolution,
                                            IRPLR,
                                            memory);
                                        double NewRoute2Cost = memory.ConcatenateSwapTwoRoutesSingleDay(
                                            pick_day,
                                            pick_vehicle2,
                                            pick_position_in_vehicle2,
                                            Swap_length2,
                                            pick_vehicle1,
                                            pick_position_in_vehicle1,
                                            Swap_length1,
                                            IRPSolution,
                                            IRPLR,
                                            memory);
                                        // cout << "NewRoute1Cost: " << NewRoute1Cost << ", NewRoute2Cost: " << NewRoute2Cost << endl;
                                        double NewTotalTransportationCost =
                                            IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle1] - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle2] + NewRoute1Cost + NewRoute2Cost;

                                        double NewLogisticRatio = std::numeric_limits<double>::max();
                                        double temp_LR_objv = std::numeric_limits<double>::max();
                                        NewLogisticRatio = NewTotalTransportationCost / IRPSolution.TotalDelivery;

                                        double NewVehicleOverload = IRPSolution.ViolationMoreThanCapacity;
                                        NewVehicleOverload = NewVehicleOverload - max(0.0, IRPSolution.VehicleLoad[pick_day][pick_vehicle1] - IRPLR.Vehicle.capacity) - max(0.0, IRPSolution.VehicleLoad[pick_day][pick_vehicle2] - IRPLR.Vehicle.capacity) 
                                        + max(0.0, temp_load_vehicle1 - IRPLR.Vehicle.capacity) + max(0.0, temp_load_vehicle2 - IRPLR.Vehicle.capacity);
                                        temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, NewVehicleOverload);

                                        cout << "objv_begin:" << objv_begin << " temp_LR_objv:" << temp_LR_objv << endl;
                                        cout << "PenaltyForStockOut: " << PenaltyForStockOut << ", PenaltyMoreThanCapacity: " << PenaltyMoreThanCapacity << endl;
                                        cout << IRPSolution.ViolationStockOut << "," << NewVehicleOverload << "," << NewTotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << NewLogisticRatio << "," << LR_objv << endl;

                                        if (objv_begin - temp_LR_objv > 0.00001)
                                        {

                                            pair_obtain_improving = 1;
                                            if (LR_objv - temp_LR_objv > 0.00001)
                                            {
                                                 // cout << "LR_objv:" << LR_objv << ", temp_LR_objv:" << temp_LR_objv << endl;
                                                LR_objv = temp_LR_objv;
                                                move[0] = pick_day;
                                                move[1] = pick_vehicle1;
                                                move[2] = pick_position_in_vehicle1;
                                                move[3] = pick_vehicle2;
                                                move[4] = pick_position_in_vehicle2;
                                                move[5] = Swap_length1;
                                                move[6] = Swap_length2;
                                                move[7] = temp_load_vehicle1;
                                                move[8] = temp_load_vehicle2;
                                                whether_improved_or_not = 1; // whether_improved_or_not = 1;

                                                ImpVehicleOverload = NewVehicleOverload;
                                                ImpLogisticRatio = NewLogisticRatio;
                                                ImpTotalTransportationCost = NewTotalTransportationCost;
                                            }
                                        }
                                        ////////////////////////////////////////////////////////////////////////
                                        //                                                                    //
                                        //      Verify the correctness of the efficient implementation        //
                                        //                                                                    //
                                        ////////////////////////////////////////////////////////////////////////

                                        // vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                                        // cout << "pick_day: " << pick_day << ", pick_vehicle1: " << pick_vehicle1 << ", vehicle1 size: " << IRPSolution.Route[pick_day][pick_vehicle1].size() << ", pick_position_in_vehicle1: " << pick_position_in_vehicle1 << ", Swap_length1: " << Swap_length1 << ", pick_vehicle2: " << pick_vehicle2 << ", vehicle2 size: " << IRPSolution.Route[pick_day][pick_vehicle2].size() << ", pick_position_in_vehicle2: " << pick_position_in_vehicle2 << ", Swap_length2: " << Swap_length2 << endl;
                                        // for (int i = 0; i < TempRoute.size(); i++)
                                        // {
                                        //     for (int j = 0; j < TempRoute[i].size(); j++)
                                        //     {
                                        //         cout << "Route for day " << i << ", vehicle " << j << ": ";
                                        //         for (int k = 0; k < TempRoute[i][j].size(); k++)
                                        //         {
                                        //             cout << TempRoute[i][j][k] << ", ";
                                        //         }
                                        //         cout << endl;
                                        //     }
                                        // }
                                        // TempRoute[pick_day][pick_vehicle1].insert(TempRoute[pick_day][pick_vehicle1].begin() + pick_position_in_vehicle1,
                                        //                                           TempRoute[pick_day][pick_vehicle2].begin() + pick_position_in_vehicle2,
                                        //                                           TempRoute[pick_day][pick_vehicle2].begin() + pick_position_in_vehicle2 + Swap_length2);
                                        // TempRoute[pick_day][pick_vehicle2].erase(TempRoute[pick_day][pick_vehicle2].begin() + pick_position_in_vehicle2,
                                        //                                          TempRoute[pick_day][pick_vehicle2].begin() + pick_position_in_vehicle2 + Swap_length2);
                                        // TempRoute[pick_day][pick_vehicle2].insert(TempRoute[pick_day][pick_vehicle2].begin() + pick_position_in_vehicle2,
                                        //                                           TempRoute[pick_day][pick_vehicle1].begin() + pick_position_in_vehicle1 + Swap_length2,
                                        //                                           TempRoute[pick_day][pick_vehicle1].begin() + pick_position_in_vehicle1 + Swap_length2 + Swap_length1);
                                        // TempRoute[pick_day][pick_vehicle1].erase(TempRoute[pick_day][pick_vehicle1].begin() + pick_position_in_vehicle1 + Swap_length2,
                                        //                                          TempRoute[pick_day][pick_vehicle1].begin() + pick_position_in_vehicle1 + Swap_length2 + Swap_length1);

                                        // double CheckRouteCost = 0;
                                        // for (int i = 0; i < TempRoute.size(); i++)
                                        // {
                                        //     for (int j = 0; j < TempRoute[i].size(); j++)
                                        //     {
                                        //         if (TempRoute[i][j].size() != 0)
                                        //         {
                                        //             CheckRouteCost += IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                                        //             CheckRouteCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                                        //             for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                                        //             {
                                        //                 CheckRouteCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                                        //             }
                                        //         }
                                        //     }
                                        // }
                                        // for (int i = 0; i < TempRoute.size(); i++)
                                        // {
                                        //     for (int j = 0; j < TempRoute[i].size(); j++)
                                        //     {
                                        //         cout << "Route for day " << i << ", vehicle " << j << ": ";
                                        //         for (int k = 0; k < TempRoute[i][j].size(); k++)
                                        //         {
                                        //             cout << TempRoute[i][j][k] << ", ";
                                        //         }
                                        //         cout << endl;
                                        //     }
                                        // }
                                        // cout << "NewTotalTransportationCost:" << NewTotalTransportationCost << ", Check Route Cost:" << CheckRouteCost << endl;
                                        // assert(fabs(NewTotalTransportationCost - CheckRouteCost) < 0.00001);
                                    }
                                    else
                                    {
                                        ////////////////////////////////////////////////////////////////////////////////
                                        //                                                                            //
                                        //      Naive implementation of evaluating the swap remove insert move        //
                                        //                                                                            //
                                        ////////////////////////////////////////////////////////////////////////////////
                                        assert(Swap_length1 > 3 && "Current code path not supported yet with Swap_length1 > 2");
                                        assert(Swap_length2 > 3 && "Current code path not supported yet with Swap_length2 > 2");
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (pair_obtain_improving == 0)
            {
                select_pair = SwapTwoRoutesOnSingleDayPair.erase(select_pair);
            }
            else
            {
                ++select_pair;
            }
        }
    }

    catch (int time_limit_reached)
    {
        cout << "!Stop by time limit" << endl;
    }
    time(&accumulate_end_time);
    accumulated_time += difftime(accumulate_end_time, accumulate_start_time);
    cout << "Accumulated time:" << accumulated_time << ";";
    cout << "Total solution explored:" << solutionCounter << ";";
    cout << "Valid insertion:" << working_solutionCounter << ";";
    cout << "whether_improved_or_not:" << whether_improved_or_not << ";";
    cout << "ImpLogisticRatio:" << ImpLogisticRatio << endl;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //       Make up the solution, adjust its inventory level and route       //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    if (whether_improved_or_not == 1) // whether_improved_or_not==1
    {
        // IRPSolution.print_solution(IRPLR);
        // cout << "Improved solution found by SwapTwoRoutesSingleDay operator:" << endl;
        // cout << ImpLogisticRatio << "\t" << ImpStockOut << endl;
        // for(int i=0;i<move.size();i++)
        // {
        //     cout<<"move["<<i<<"]:"<<move[i]<<"\t";
        // }
        // cout<<endl;
        IRPSolution.VehicleLoad[move[0]][move[1]] = move[7];
        IRPSolution.VehicleLoad[move[0]][move[3]] = move[8];
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.TotalTransportationCost = ImpTotalTransportationCost;
        IRPSolution.ViolationMoreThanCapacity = ImpVehicleOverload;

        for (int route2_index = 0; route2_index < move[6]; route2_index++)
        {

            for (int day = move[0]; day < IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]].size(); day++)
            {
                if (IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]][day] < IRPLR.NumberOfVehicles) // For vehicle j that visits this customer on day i
                {
                    for (int vehicle_1 = 0; vehicle_1 < IRPSolution.Route[day].size(); vehicle_1++) // For routes on day i
                    {
                        for (int vehicle_2 = 0; vehicle_2 < IRPSolution.Route[day].size(); vehicle_2++) // For customers on route j on day i
                        {
                            if (vehicle_2 > vehicle_1)
                            {
                                vector<int> temp_pair;
                                temp_pair.push_back(day);
                                temp_pair.push_back(vehicle_1);
                                temp_pair.push_back(vehicle_2);
                                SwapTwoRoutesOnSingleDayPairToReconsider.insert(temp_pair);
                            }
                        }
                    }
                }
            }
        }
        for (int route1_index = 0; route1_index < move[5]; route1_index++)
        {

            for (int day = move[0]; day < IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[1]][move[2] + route1_index]].size(); day++)
            {
                if (IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[1]][move[2] + route1_index]][day] < IRPLR.NumberOfVehicles)
                {
                    for (int vehicle_1 = 0; vehicle_1 < IRPSolution.Route[day].size(); vehicle_1++)
                    {
                        for (int vehicle_2 = 0; vehicle_2 < IRPSolution.Route[day].size(); vehicle_2++)
                        {
                            if (vehicle_2 > vehicle_1)
                            {
                                vector<int> temp_pair;
                                temp_pair.push_back(day);
                                temp_pair.push_back(vehicle_1);
                                temp_pair.push_back(vehicle_2);
                                SwapTwoRoutesOnSingleDayPairToReconsider.insert(temp_pair);
                            }
                        }
                    }
                }
            }
        }

        IRPSolution.Route[move[0]][move[1]].insert(IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                   IRPSolution.Route[move[0]][move[3]].begin() + move[4],
                                                   IRPSolution.Route[move[0]][move[3]].begin() + move[4] + move[6]);
        IRPSolution.Route[move[0]][move[3]].erase(IRPSolution.Route[move[0]][move[3]].begin() + move[4],
                                                  IRPSolution.Route[move[0]][move[3]].begin() + move[4] + move[6]);
        IRPSolution.Route[move[0]][move[3]].insert(IRPSolution.Route[move[0]][move[3]].begin() + move[4],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6] + move[5]);
        IRPSolution.Route[move[0]][move[1]].erase(IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6],
                                                  IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6] + move[5]);
        memory.TrackSolutionStatus[move[0]][move[1]] = 1;          // Mark route as changed
        memory.TrackSolutionStatus[move[0]][move[3]] = 1;          // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[0]][move[1]] = 1; // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[0]][move[3]] = 1; // Mark route as changed
        IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
        IRPSolution.print_solution(IRPLR);
        ////////////////////////////////////////////////////////////////////////
        //                                                                    //
        //             Verify the correctness of the output                   //
        //                                                                    //
        ////////////////////////////////////////////////////////////////////////

        // double CheckRouteCost = 0;
        // for (int i = 0; i < IRPSolution.Route.size(); i++)
        // {
        //     for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        //     {
        //         if (IRPSolution.Route[i][j].size() != 0)
        //         {
        //             CheckRouteCost += IRPLR.Distance[0][IRPSolution.Route[i][j][0] + 1];
        //             CheckRouteCost += IRPLR.Distance[IRPSolution.Route[i][j][IRPSolution.Route[i][j].size() - 1] + 1][0];
        //             for (int test = 0; test < IRPSolution.Route[i][j].size() - 1; test++)
        //             {
        //                 CheckRouteCost += IRPLR.Distance[IRPSolution.Route[i][j][test] + 1][IRPSolution.Route[i][j][test + 1] + 1];
        //             }
        //         }
        //     }
        // }
        // cout << "ImpTotalTransportationCost:" << ImpTotalTransportationCost << "\t CheckRouteCost:" << CheckRouteCost << endl;
        // assert(fabs(ImpTotalTransportationCost - CheckRouteCost) < 0.00001);

        // double CheckTotalDelivery = 0;
        // for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
        // {

        //     for (int j = 0; j < IRPSolution.DeliveryQuantity[i].size(); j++)
        //     {
        //         CheckTotalDelivery += IRPSolution.DeliveryQuantity[i][j];
        //         // cout<<IRPSolution.DeliveryQuantity[i][j]<<",";
        //     }
        //     // cout << endl;
        //     // cout<<"Customer "<<i<<", total delivery after move:"<<total_delivery_for_customer<<endl;
        // }
        // cout << "ImpTotalDelivery:" << ImpTotalDelivery << "\t CheckTotalDelivery:" << CheckTotalDelivery << endl;
        // IRPSolution.print_solution(IRPLR);
        // assert(fabs(ImpTotalDelivery - CheckTotalDelivery) < 0.00001);
    }
    cout << "================================" << endl;
    return whether_improved_or_not;
}