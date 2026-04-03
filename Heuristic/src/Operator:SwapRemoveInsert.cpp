#include "lib.h"

int solution_improvement::OperatorSwapRemoveInsert(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory, set<vector<int>> &SwapRemoveInsertPair,
                                                   int &min_remove_length, int &max_remove_length,
                                                   int &min_insert_length, int &max_insert_length)
{

    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut: " << IRPSolution.ViolationStockOut << endl;

    // IRPSolution.print_solution(IRPLR);
    // for(int i=0; i<IRPSolution.TransportationCostPerRoute.size(); i++)
    // {
    //     for(int j=0; j<IRPSolution.TransportationCostPerRoute[i].size(); j++)
    //     {
    //         cout<<"TransportationCostPerRoute["<<i<<"]["<<j<<"]:"<<IRPSolution.TransportationCostPerRoute[i][j]<<",";
    //     }
    //     cout << endl;
    // }
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut);
    // cout << "IRPSolution.TotalTransportationCost:"<<IRPSolution.TotalTransportationCost<<", IRPSolution.TotalDelivery:"<<IRPSolution.TotalDelivery<<", LR objv:" << LR_objv << endl;
    vector<int> move;
    for (int i = 0; i < 7; i++)
    {
        move.push_back(0);
        // move[0]: pick_day
        // move[1]: pick_vehicle
        // move[2]: pick_allocated_position
        // move[3]: pick_unallocated_position
        // move[4]: remove_length
        // move[5]: insert_length
    }

    int whether_improved_or_not = 0;
    vector<vector<double>> ImpDeliveryQuantityCustomerRemove;
    vector<vector<double>> ImpInventoryLevelCustomerRemove;
    vector<vector<double>> ImpDeliveryQuantityCustomerInsert;
    vector<vector<double>> ImpInventoryLevelCustomerInsert;
    vector<vector<double>> ImpVehicleLoad;
    double ImpStockOut = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;
    int solutionCounter = 0;
    int naive_implementation = 0;
    set<vector<int>>::iterator select_pair;
    time(&accumulate_start_time);
    try
    {
        for (select_pair = SwapRemoveInsertPair.begin(); select_pair != SwapRemoveInsertPair.end(); select_pair++)
        {
            const vector<int> &pair = (*select_pair);
            int pick_day = pair[0];
            int pick_vehicle = pair[1];
            int pair_obtain_improving = 0;

            for (int pick_allocated_position = 0; pick_allocated_position <= IRPSolution.Route[pick_day][pick_vehicle].size(); pick_allocated_position++) // A position in this vehicle
            {
                for (int pick_unallocated_position = 0; pick_unallocated_position <= IRPSolution.UnallocatedCustomers[pick_day].size(); pick_unallocated_position++) // A position in the unallocated customer
                {
                    for (int remove_length = min_remove_length; remove_length <= max_remove_length; remove_length++)
                    {
                        for (int insert_length = min_insert_length; insert_length <= max_insert_length; insert_length++)
                        {
                            if (remove_length != 0 || insert_length != 0)
                            {
                                if (remove_length != 0 && insert_length == 0 && pick_unallocated_position > 0)
                                {
                                    // For purely remove customers, the position of the removed customers in the unallocated customer list does not matter
                                    // So we assume the removed customers are always from the beginning of the unallocated customer list
                                    // This logical operations skips the evaluation of inserting the removed customers to the subsequent positions in the unallocated customer list
                                }
                                else
                                {
                                    if (pick_unallocated_position + insert_length <= IRPSolution.UnallocatedCustomers[pick_day].size() && pick_allocated_position + remove_length <= IRPSolution.Route[pick_day][pick_vehicle].size()) // Make sure the operator does not go outside the range
                                    {
                                        time(&LS_end_time);
                                        double total_ls_time = difftime(LS_end_time, LS_start_time);
                                        if (total_ls_time - LocalSearchTimeLimit > 0.00001)
                                        {
                                            int time_limit_reached = total_ls_time;
                                            throw time_limit_reached;
                                        }
                                        if (insert_length <= 2 && remove_length <= 2)
                                        {
                                            ////////////////////////////////////////////////////////////////////////////////
                                            //                                                                            //
                                            //      Efficient implementation of evaluating the swap remove insert move    //
                                            //                                                                            //
                                            ////////////////////////////////////////////////////////////////////////////////
                                            solutionCounter++;
                                            /////////////////////////////////////////////////////////////////////////////////
                                            // Initialize all data needed to capture the improving solution
                                            vector<vector<double>> NewDeliveryQuantityCustomerRemove;
                                            vector<vector<double>> NewInventoryLevelCustomerRemove;
                                            for (int remove_index = pick_allocated_position; remove_index < pick_allocated_position + remove_length; remove_index++)
                                            {
                                                NewDeliveryQuantityCustomerRemove.push_back(IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle][remove_index]]);
                                                NewInventoryLevelCustomerRemove.push_back(IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle][remove_index]]);
                                            }
                                            assert(NewDeliveryQuantityCustomerRemove.size() == remove_length);
                                            assert(NewInventoryLevelCustomerRemove.size() == remove_length);
                                            vector<vector<double>> NewDeliveryQuantityCustomerInsert;
                                            vector<vector<double>> NewInventoryLevelCustomerInsert;
                                            for (int insert_index = pick_unallocated_position; insert_index < pick_unallocated_position + insert_length; insert_index++)
                                            {
                                                NewDeliveryQuantityCustomerInsert.push_back(IRPSolution.DeliveryQuantity[IRPSolution.UnallocatedCustomers[pick_day][insert_index]]);
                                                NewInventoryLevelCustomerInsert.push_back(IRPSolution.InventoryLevel[IRPSolution.UnallocatedCustomers[pick_day][insert_index]]);
                                            }
                                            assert(NewDeliveryQuantityCustomerInsert.size() == insert_length);
                                            assert(NewInventoryLevelCustomerInsert.size() == insert_length);

                                            double NewStockOut = IRPSolution.ViolationStockOut;
                                            double ChangeInTotalQuantity = 0;
                                            vector<vector<double>> CopyVehicleLoad = IRPSolution.VehicleLoad;
                                            // cout << "Selected Vehicle load for picked day " << SelectedVehicleLoad << endl;

                                            /////////////////////////////////////////////////////////////////////////////////
                                            // Get change in delivery quantity and inventory level for the removed customers
                                            for (int remove_index = 0; remove_index < remove_length; remove_index++)
                                            {
                                                CopyVehicleLoad[pick_day][pick_vehicle] = CopyVehicleLoad[pick_day][pick_vehicle] - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index]][pick_day];
                                                NewDeliveryQuantityCustomerRemove[remove_index][pick_day] = 0;
                                                ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index]][pick_day];

                                                // The following code adjust the delivery quantity and inventory level of the removed customer across all affected periods.
                                                if (pick_day == 0)
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index]].InventoryBegin,
                                                        pick_day,
                                                        pick_vehicle,
                                                        NewDeliveryQuantityCustomerRemove[remove_index],
                                                        NewInventoryLevelCustomerRemove[remove_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index],
                                                        IRPLR);
                                                }
                                                else
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index]][pick_day - 1],
                                                        pick_day,
                                                        pick_vehicle,
                                                        NewDeliveryQuantityCustomerRemove[remove_index],
                                                        NewInventoryLevelCustomerRemove[remove_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index],
                                                        IRPLR);
                                                }
                                                // cout << "SelectedVehicleLoad: " << SelectedVehicleLoad << ", ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                                // cout << "NewDeliveryQuantityCustomerRemove" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityCustomerRemove.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityCustomerRemove[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityCustomerRemove[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelCustomerRemove" << endl;
                                                // for (int i = 0; i < NewInventoryLevelCustomerRemove.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelCustomerRemove[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelCustomerRemove[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewDeliveryQuantityCustomerInsert" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityCustomerInsert.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityCustomerInsert[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityCustomerInsert[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelCustomerInsert" << endl;
                                                // for (int i = 0; i < NewInventoryLevelCustomerInsert.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelCustomerInsert[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelCustomerInsert[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                            }
                                            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                            // Get change in delivery quantity and inventory level for the inserted customers
                                            int Whether_insert_fail = 0; // if insert does not lead to an increase on delivery quantity, this move fails, therefore skip the evalution
                                            for (int insert_index = 0; insert_index < insert_length; insert_index++)
                                            {
                                                 if (pick_day == 0)
                                                 {
                                                     NewDeliveryQuantityCustomerInsert[insert_index][pick_day] = DeliveryMax(
                                                         IRPLR.Vehicle.capacity,
                                                         CopyVehicleLoad[pick_day][pick_vehicle],
                                                         IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].InventoryMax,
                                                         IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].Demand,
                                                         IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].InventoryBegin);
                                                 }
                                                 else
                                                 {
                                                     NewDeliveryQuantityCustomerInsert[insert_index][pick_day] = DeliveryMax(
                                                         IRPLR.Vehicle.capacity,
                                                         CopyVehicleLoad[pick_day][pick_vehicle],
                                                         IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].InventoryMax,
                                                         IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].Demand,
                                                         IRPSolution.InventoryLevel[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]][pick_day - 1]);
                                                 }

                                                if (NewDeliveryQuantityCustomerInsert[insert_index][pick_day] < 0.001)
                                                {
                                                    Whether_insert_fail = 1;
                                                }
                                                else
                                                {
                                                    ChangeInTotalQuantity += NewDeliveryQuantityCustomerInsert[insert_index][pick_day] - IRPSolution.DeliveryQuantity[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]][pick_day];
                                                    CopyVehicleLoad[pick_day][pick_vehicle] = CopyVehicleLoad[pick_day][pick_vehicle] + NewDeliveryQuantityCustomerInsert[insert_index][pick_day];

                                                    assert(NewDeliveryQuantityCustomerInsert[insert_index][pick_day] != 0);

                                                    if (pick_day == 0)
                                                    {
                                                        AdjustQuantityAndInventoryLevel(
                                                            IRPLR.Retailers[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]].InventoryBegin,
                                                            pick_day,
                                                            pick_vehicle,
                                                            NewDeliveryQuantityCustomerInsert[insert_index],
                                                            NewInventoryLevelCustomerInsert[insert_index],
                                                            CopyVehicleLoad,
                                                            IRPSolution.VehicleAllocation,
                                                            ChangeInTotalQuantity,
                                                            NewStockOut,
                                                            IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index],
                                                            IRPLR);
                                                    }
                                                    else
                                                    {
                                                        AdjustQuantityAndInventoryLevel(
                                                            IRPSolution.InventoryLevel[IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index]][pick_day - 1],
                                                            pick_day,
                                                            pick_vehicle,
                                                            NewDeliveryQuantityCustomerInsert[insert_index],
                                                            NewInventoryLevelCustomerInsert[insert_index],
                                                            CopyVehicleLoad,
                                                            IRPSolution.VehicleAllocation,
                                                            ChangeInTotalQuantity,
                                                            NewStockOut,
                                                            IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index],
                                                            IRPLR);
                                                    }
                                                }
                                            }

                                            if (Whether_insert_fail == 0)
                                            {

                                                // cout << "SelectedVehicleLoad: " << SelectedVehicleLoad << ", ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                                // cout << "NewDeliveryQuantityCustomerRemove" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityCustomerRemove.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityCustomerRemove[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityCustomerRemove[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelCustomerRemove" << endl;
                                                // for (int i = 0; i < NewInventoryLevelCustomerRemove.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelCustomerRemove[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelCustomerRemove[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewDeliveryQuantityCustomerInsert" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityCustomerInsert.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityCustomerInsert[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityCustomerInsert[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelCustomerInsert" << endl;
                                                // for (int i = 0; i < NewInventoryLevelCustomerInsert.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelCustomerInsert[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelCustomerInsert[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;

                                                double NewRouteCost = memory.ConcatenateSwapInsertRemove(pick_day, pick_vehicle, pick_allocated_position, remove_length,
                                                                                                         pick_unallocated_position, insert_length,
                                                                                                         IRPSolution, IRPLR, memory);
                                                double NewTotalTransportationCost = IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle] + NewRouteCost;
                                                double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;
                                                double NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                                                double temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, NewStockOut);

                                                if (LR_objv - temp_LR_objv > 0.00001)
                                                {

                                                    // cout << "LR_objv:" << LR_objv << ", temp_LR_objv:" << temp_LR_objv << endl;
                                                    LR_objv = temp_LR_objv;
                                                    move[0] = pick_day;
                                                    move[1] = pick_vehicle;
                                                    move[2] = pick_allocated_position;
                                                    move[3] = pick_unallocated_position;
                                                    move[4] = remove_length;
                                                    move[5] = insert_length;
                                                    whether_improved_or_not = 1; // whether_improved_or_not = 1;
                                                    ImpVehicleLoad = CopyVehicleLoad;
                                                    ImpDeliveryQuantityCustomerRemove.clear();
                                                    ImpInventoryLevelCustomerRemove.clear();
                                                    ImpDeliveryQuantityCustomerInsert.clear();
                                                    ImpInventoryLevelCustomerInsert.clear();

                                                    for (int remove_index = 0; remove_index < remove_length; remove_index++)
                                                    {
                                                        ImpDeliveryQuantityCustomerRemove.push_back(NewDeliveryQuantityCustomerRemove[remove_index]);
                                                        ImpInventoryLevelCustomerRemove.push_back(NewInventoryLevelCustomerRemove[remove_index]);
                                                    }

                                                    for (int insert_index = 0; insert_index < insert_length; insert_index++)
                                                    {
                                                        ImpDeliveryQuantityCustomerInsert.push_back(NewDeliveryQuantityCustomerInsert[insert_index]);
                                                        ImpInventoryLevelCustomerInsert.push_back(NewInventoryLevelCustomerInsert[insert_index]);
                                                    }

                                                    ImpStockOut = NewStockOut;
                                                    ImpLogisticRatio = NewLogisticRatio;
                                                    ImpTotalTransportationCost = NewTotalTransportationCost;
                                                    ImpTotalDelivery = NewTotalDelivery;

                                                    // cout << "!Improving solution found" << endl;
                                                }

                                                ////////////////////////////////////////////////////////////////////////
                                                //                                                                    //
                                                //      Verify the correctness of the efficient implementation        //
                                                //                                                                    //
                                                ////////////////////////////////////////////////////////////////////////

                                                // vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                                                // vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                                                // TempRoute[pick_day][pick_vehicle].insert(TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_position,
                                                //                                          TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_position,
                                                //                                          TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_position + insert_length);
                                                // TempUnallocatedCustomers[pick_day].erase(TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_position,
                                                //                                          TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_position + insert_length);
                                                // TempUnallocatedCustomers[pick_day].insert(TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_position,
                                                //                                           TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_position + insert_length,
                                                //                                           TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_position + insert_length + remove_length);
                                                // TempRoute[pick_day][pick_vehicle].erase(TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_position + insert_length,
                                                //                                         TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_position + insert_length + remove_length);

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

                                                // cout << "NewTotalTransportationCost:" << NewTotalTransportationCost << ", Check Route Cost:" << CheckRouteCost << endl;
                                                // assert(fabs(NewTotalTransportationCost - CheckRouteCost) < 0.00001);
                                            }
                                        }
                                        else
                                        {
                                            ////////////////////////////////////////////////////////////////////////////////
                                            //                                                                            //
                                            //      Naive implementation of evaluating the swap remove insert move        //
                                            //                                                                            //
                                            ////////////////////////////////////////////////////////////////////////////////
                                            assert(insert_length > 2 && "Current code path not supported yet with insert_length > 2");
                                            assert(remove_length > 2 && "Current code path not supported yet with remove_length > 2");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // if (pair_obtain_improving == 0)
            // {
            //     select_pair = SwapRemoveInsertPair.erase(select_pair);
            // }
            // else
            // {
            //     ++select_pair;
            // }
        }
    }
    catch (int time_limit_reached)
    {
        cout << "!Stop by time limit" << endl;
    }
    time(&accumulate_end_time);
    accumulated_time += difftime(accumulate_end_time, accumulate_start_time);
    cout << "Accumulated time:" << accumulated_time << endl;
    cout << "Total solution explored:" << solutionCounter << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    cout << "ImpLogisticRatio:" << ImpLogisticRatio << "\t ImpStockOut:" << ImpStockOut << endl;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //       Make up the solution, adjust its inventory level and route       //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    if (whether_improved_or_not == 1) // whether_improved_or_not==1
    {
        // cout<<ImpLogisticRatio<<"\t"<<ImpStockOut<<endl;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;
        for (int insert_index = 0; insert_index < move[5]; insert_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index]] = ImpDeliveryQuantityCustomerInsert[insert_index];
            IRPSolution.InventoryLevel[IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index]] = ImpInventoryLevelCustomerInsert[insert_index];
            IRPSolution.VehicleAllocation[IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index]][move[0]] = move[1];
        }
        for (int remove_index = 0; remove_index < move[4]; remove_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]] = ImpDeliveryQuantityCustomerRemove[remove_index];
            IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]] = ImpInventoryLevelCustomerRemove[remove_index];
            IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]][move[0]] = IRPLR.NumberOfVehicles + 1; // Mark as unallocated
        }

        IRPSolution.Route[move[0]][move[1]].insert(IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                   IRPSolution.UnallocatedCustomers[move[0]].begin() + move[3],
                                                   IRPSolution.UnallocatedCustomers[move[0]].begin() + move[3] + move[5]);
        IRPSolution.UnallocatedCustomers[move[0]].erase(IRPSolution.UnallocatedCustomers[move[0]].begin() + move[3],
                                                        IRPSolution.UnallocatedCustomers[move[0]].begin() + move[3] + move[5]);
        IRPSolution.UnallocatedCustomers[move[0]].insert(IRPSolution.UnallocatedCustomers[move[0]].begin() + move[3],
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[5],
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[5] + move[4]);
        IRPSolution.Route[move[0]][move[1]].erase(IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[5],
                                                  IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[5] + move[4]);
        memory.TrackSolutionStatus[move[0]][move[1]] = 1;          // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[0]][move[1]] = 1; // Mark route as changed
        IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
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
        // cout<<"ImpTotalTransportationCost:"<<ImpTotalTransportationCost<<"\t CheckRouteCost:"<<CheckRouteCost<<endl;
        // assert(fabs(ImpTotalTransportationCost - CheckRouteCost) < 0.00001);

        // double CheckTotalDelivery = 0;
        // for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
        // {

        //     for (int j = 0; j < IRPSolution.DeliveryQuantity[i].size(); j++)
        //     {
        //         CheckTotalDelivery += IRPSolution.DeliveryQuantity[i][j];
        //     }
        //     // cout<<"Customer "<<i<<", total delivery after move:"<<total_delivery_for_position<<endl;
        // }
        // cout<<"ImpTotalDelivery:"<<ImpTotalDelivery<<"\t CheckTotalDelivery:"<<CheckTotalDelivery<<endl;
        // assert(fabs(ImpTotalDelivery - CheckTotalDelivery) < 0.00001);
    }

    return whether_improved_or_not;
}