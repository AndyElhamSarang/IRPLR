#include "lib.h"

int solution_improvement::OperatorTransfer(
    input &IRPLR,
    solution &IRPSolution,
    double &PenaltyForStockOut,
    vector<vector<int>> &TransferDetails,
    preprocessing &memory)
{ // Current implementation only permits at most one customer therefore

    int TransferNumber = 1;
    assert(TransferNumber <= 1);

    cout << "Transfer starting solution" << endl;
    int whether_improved_or_not = 0;
    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    // IRPSolution.print_solution(IRPLR);
    // cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut);
    // cout << "LR objv:" << LR_objv << endl;
    double objv_begin = LR_objv;
    vector<double> ImpDeliveryQuantityCustomerTransfer;
    vector<double> ImpInventoryLevelCustomerTransfer;
    vector<vector<double>> ImpVehicleLoad;
    double ImpStockOut = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;

    vector<int> move;
    for (int i = 0; i < 9; i++)
    {
        move.push_back(0);
        // move[0]: pick_day1
        // move[1]: pick_vehicle1
        // move[2]: pick_position_in_vehicle1
        // move[3]: pick_day2
        // move[4]: pick_vehicle2
        // move[5]: pick_position_in_vehicle2
        // move[6]: Swap_length1
        // move[7]: Swap_length2
        // move[8]: customer_index
    }

    int solutionCounter = 0;
    int working_solutionCounter = 0;
    int naive_implementation = 0;

    time(&accumulate_start_time);
    try
    {

        for (int i = 0; i < TransferDetails.size(); i++)
        {
            int pick_day_removed = TransferDetails[i][1];
            int pick_vehicle_added = TransferDetails[i][2];

            time(&total_end_time);
            double total_ls_time = difftime(total_end_time, total_start_time);
            if (total_ls_time - MainAlgorithmTimeLimit > 0.00001)
            {
                int time_limit_reached = total_ls_time;
                throw time_limit_reached;
            }
            ////////////////////////////////////////////////////////////////////////////////
            //                                                                            //
            //      Efficient implementation of evaluating the swap remove insert move    //
            //                                                                            //
            ////////////////////////////////////////////////////////////////////////////////
            solutionCounter++;
            /////////////////////////////////////////////////////////////////////////////////
            // Initialize all data needed to capture the improving solution
            vector<double> NewDeliveryQuantityCustomerTransfer(IRPSolution.DeliveryQuantity[TransferDetails[i][0]]);
            vector<double> NewInventoryLevelCustomerTransfer(IRPSolution.InventoryLevel[TransferDetails[i][0]]);
            double NewStockOut = IRPSolution.ViolationStockOut;
            double ChangeInTotalQuantity = 0;
            vector<vector<double>> CopyVehicleLoad = IRPSolution.VehicleLoad;
            // cout << "================================================================" << endl;
            // cout << "TransferDetails: Retailer " << TransferDetails[i][0] << ", from day " << TransferDetails[i][1] << " to day " << TransferDetails[i][2] << endl;
            // cout << "NewDeliveryQuantityCustomerTransfer:";
            // for (int period = 0; period < NewDeliveryQuantityCustomerTransfer.size(); period++)
            // {
            //     cout << NewDeliveryQuantityCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            // cout << "NewInventoryLevelCustomerTransfer:";
            // for (int period = 0; period < NewInventoryLevelCustomerTransfer.size(); period++)
            // {
            //     cout << NewInventoryLevelCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            /////////////////////////////////////////////////////////////////////////////////
            // Get change in delivery quantity and inventory level for the removed customers

            CopyVehicleLoad[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]] = CopyVehicleLoad[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]] - IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][1]];
            NewDeliveryQuantityCustomerTransfer[TransferDetails[i][1]] = 0;
            ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][1]];

            // The following code adjust the delivery quantity and inventory level of the removed customer across all affected periods.
            if (TransferDetails[i][1] == 0)
            {
                AdjustQuantityAndInventoryLevel(
                    IRPLR.Retailers[TransferDetails[i][0]].InventoryBegin,
                    TransferDetails[i][1],
                    IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    TransferDetails[i][0],
                    IRPLR);
            }
            else
            {
                AdjustQuantityAndInventoryLevel(
                    IRPSolution.InventoryLevel[TransferDetails[i][0]][TransferDetails[i][1] - 1],
                    TransferDetails[i][1],
                    IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    TransferDetails[i][0],
                    IRPLR);
            }
            // cout << "--------------------------------------------------------------------------------" << endl;
            // cout << "After removing the customer from the original day, the change in total quantity is: " << ChangeInTotalQuantity << ", and the new stock out is: " << NewStockOut << endl;

            // cout << "NewDeliveryQuantityCustomerTransfer:";
            // for (int period = 0; period < NewDeliveryQuantityCustomerTransfer.size(); period++)
            // {
            //     cout << NewDeliveryQuantityCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            // cout << "NewInventoryLevelCustomerTransfer:";
            // for (int period = 0; period < NewInventoryLevelCustomerTransfer.size(); period++)
            // {
            //     cout << NewInventoryLevelCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            int Whether_insert_fail = 0; // if insert does not lead to an increase on delivery quantity, this move fails, therefore skip the evalution
            ////////////////////////////////////////////////////////////////////////////////////
            //                                                                                //
            //                Insert to a vehicle with largest delivery quantity              //
            //                                                                                //
            ////////////////////////////////////////////////////////////////////////////////////

            NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]] = 0;
            int VehicleIndexInserted = IRPLR.NumberOfVehicles + 1;
            for (int VehicleIndexNewDay = 0; VehicleIndexNewDay < IRPLR.NumberOfVehicles; VehicleIndexNewDay++)
            {
                double temp_NewDeliveryQuantityCustomerTransfer = 0;
                if (TransferDetails[i][2] == 0)
                {
                    temp_NewDeliveryQuantityCustomerTransfer = DeliveryMax(
                        IRPLR.Vehicle.capacity,
                        CopyVehicleLoad[TransferDetails[i][2]][VehicleIndexNewDay],
                        IRPLR.Retailers[TransferDetails[i][0]].InventoryMax,
                        IRPLR.Retailers[TransferDetails[i][0]].Demand,
                        IRPLR.Retailers[TransferDetails[i][0]].InventoryBegin);
                }
                else
                {
                    temp_NewDeliveryQuantityCustomerTransfer = DeliveryMax(
                        IRPLR.Vehicle.capacity,
                        CopyVehicleLoad[TransferDetails[i][2]][VehicleIndexNewDay],
                        IRPLR.Retailers[TransferDetails[i][0]].InventoryMax,
                        IRPLR.Retailers[TransferDetails[i][0]].Demand,
                        NewInventoryLevelCustomerTransfer[TransferDetails[i][2] - 1]);
                }
                if (temp_NewDeliveryQuantityCustomerTransfer > NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]])
                {
                    NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]] = temp_NewDeliveryQuantityCustomerTransfer;
                    VehicleIndexInserted = VehicleIndexNewDay;
                }
            }
            if (NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]] < 0.001)
            {
                Whether_insert_fail = 1;
                assert(VehicleIndexInserted == IRPLR.NumberOfVehicles + 1);
            }
            else
            {
                assert(NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]] > 0.001);
                assert(IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][2]] < 0.001);
                ChangeInTotalQuantity += NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]] - IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][2]];
                CopyVehicleLoad[TransferDetails[i][2]][VehicleIndexInserted] = CopyVehicleLoad[TransferDetails[i][2]][VehicleIndexInserted] + NewDeliveryQuantityCustomerTransfer[TransferDetails[i][2]];

                if (TransferDetails[i][2] == 0)
                {
                    AdjustQuantityAndInventoryLevel(
                        IRPLR.Retailers[TransferDetails[i][0]].InventoryBegin,
                        TransferDetails[i][2],
                        VehicleIndexInserted,
                        NewDeliveryQuantityCustomerTransfer,
                        NewInventoryLevelCustomerTransfer,
                        CopyVehicleLoad,
                        IRPSolution.VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        TransferDetails[i][0],
                        IRPLR);
                }
                else
                {
                    AdjustQuantityAndInventoryLevel(
                        NewInventoryLevelCustomerTransfer[TransferDetails[i][2] - 1],
                        TransferDetails[i][2],
                        VehicleIndexInserted,
                        NewDeliveryQuantityCustomerTransfer,
                        NewInventoryLevelCustomerTransfer,
                        CopyVehicleLoad,
                        IRPSolution.VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        TransferDetails[i][0],
                        IRPLR);
                }
            }

            // cout << "--------------------------------------------------------------------------------" << endl;
            // cout << "After adding the customer from the original day to the new day, " << "at vehicle " << VehicleIndexInserted << ", the change in total quantity is: " << ChangeInTotalQuantity << ", and the new stock out is: " << NewStockOut << endl;

            // cout << "NewDeliveryQuantityCustomerTransfer:";
            // for (int period = 0; period < NewDeliveryQuantityCustomerTransfer.size(); period++)
            // {
            //     cout << NewDeliveryQuantityCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            // cout << "NewInventoryLevelCustomerTransfer:";
            // for (int period = 0; period < NewInventoryLevelCustomerTransfer.size(); period++)
            // {
            //     cout << NewInventoryLevelCustomerTransfer[period] << ",";
            // }
            // cout << endl;

            if (Whether_insert_fail == 0)
            {
                working_solutionCounter++;
                int temp_int = IRPLR.NumberOfVehicles + 1;
                int temp_int2 = 0;
                double NewRouteCostRemoved = memory.ConcatenateSwapTwoRoutesSingleDay(
                    TransferDetails[i][1],
                    IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                    IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]],
                    TransferNumber,
                    temp_int,
                    temp_int2,
                    temp_int2,
                    IRPSolution,
                    IRPLR,
                    memory);
                int CheapestInsertPosition = IRPSolution.Route[TransferDetails[i][2]][VehicleIndexInserted].size() + 1;
                double NewRouteCostInserted = numeric_limits<double>::max();
                for (int insert_position = 0; insert_position <= IRPSolution.Route[TransferDetails[i][2]][VehicleIndexInserted].size(); insert_position++) // A position in the new vehicle
                {

                    double TempNewRouteCostInserted = memory.ConcatenateChepestInsertion(
                        TransferDetails[i][2],
                        VehicleIndexInserted,
                        insert_position,
                        temp_int2,
                        TransferDetails[i][1],
                        IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                        IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]],
                        TransferNumber,
                        IRPSolution,
                        IRPLR,
                        memory);
                    if (TempNewRouteCostInserted < NewRouteCostInserted)
                    {
                        NewRouteCostInserted = TempNewRouteCostInserted;
                        CheapestInsertPosition = insert_position;
                    }
                }
                assert(CheapestInsertPosition < IRPSolution.Route[TransferDetails[i][2]][VehicleIndexInserted].size() + 1);

                double NewTotalTransportationCost =
                    IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]] + NewRouteCostRemoved - IRPSolution.TransportationCostPerRoute[TransferDetails[i][2]][VehicleIndexInserted] + NewRouteCostInserted;

                double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;
                double NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                double temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, NewStockOut);

                if (objv_begin - temp_LR_objv > 0.00001)
                {

                    if (LR_objv - temp_LR_objv > 0.00001)
                    {
                        whether_improved_or_not = 1;
                        LR_objv = temp_LR_objv;

                        move[0] = TransferDetails[i][1];
                        move[1] = IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]];
                        move[2] = IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]];
                        move[3] = TransferDetails[i][2];
                        move[4] = VehicleIndexInserted;
                        move[5] = CheapestInsertPosition;
                        move[6] = TransferNumber;
                        move[7] = 0;
                        move[8] = TransferDetails[i][0];
                        ImpDeliveryQuantityCustomerTransfer.clear();
                        ImpInventoryLevelCustomerTransfer.clear();
                        ImpDeliveryQuantityCustomerTransfer = NewDeliveryQuantityCustomerTransfer;
                        ImpInventoryLevelCustomerTransfer = NewInventoryLevelCustomerTransfer;
                        ImpVehicleLoad = CopyVehicleLoad;
                        ImpStockOut = NewStockOut;
                        ImpLogisticRatio = NewLogisticRatio;
                        ImpTotalTransportationCost = NewTotalTransportationCost;
                        ImpTotalDelivery = NewTotalDelivery;
                    }
                }

                ////////////////////////////////////////////////////////////////////////
                //                                                                    //
                //      Verify the correctness of the efficient implementation        //
                //                                                                    //
                ////////////////////////////////////////////////////////////////////////

                // vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                // cout << "Customer " << TransferDetails[i][0] << ", "
                //      << "remove_day: " << TransferDetails[i][1] << ", remove from vehicle: " << IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]
                //      << ", remove position: " << IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]] << ", remove_length: " << TransferNumber
                //      << ", insert_day: " << TransferDetails[i][2] << ", insert vehicle: " << VehicleIndexInserted << ", insert_position: " << CheapestInsertPosition << ", insert_length: " << TransferNumber << endl;
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
                // TempRoute[TransferDetails[i][2]][VehicleIndexInserted].insert(TempRoute[TransferDetails[i][2]][VehicleIndexInserted].begin() + CheapestInsertPosition,
                //                                                               TempRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]].begin() + IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]],
                //                                                               TempRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]].begin() + IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]] + TransferNumber);
                // TempRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]].erase(
                //     TempRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]].begin() + IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]],
                //     TempRoute[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]].begin() + IRPSolution.VisitOrder[TransferDetails[i][0]][TransferDetails[i][1]] + TransferNumber);

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
    cout << "Valid insertion:" << working_solutionCounter << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    cout << "ImpLogisticRatio:" << ImpLogisticRatio << "\t ImpStockOut:" << ImpStockOut << endl;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //       Make up the solution, adjust its inventory level and route       //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    if (whether_improved_or_not == 1) // whether_improved_or_not==1
    {
        // IRPSolution.print_solution(IRPLR);
        // for(int i=0;i<move.size();i++)
        // {
        //     cout<<"move["<<i<<"]:"<<move[i]<<"\t";
        // }
        // cout<<endl;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;

        IRPSolution.DeliveryQuantity[move[8]] = ImpDeliveryQuantityCustomerTransfer;
        IRPSolution.InventoryLevel[move[8]] = ImpInventoryLevelCustomerTransfer;

        IRPSolution.UnallocatedCustomers[move[0]].insert(IRPSolution.UnallocatedCustomers[move[0]].begin(),
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        IRPSolution.Route[move[3]][move[4]].insert(IRPSolution.Route[move[3]][move[4]].begin() + move[5],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        IRPSolution.Route[move[0]][move[1]].erase(IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                  IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        bool whether_remove_inserted_customer_exists = false;
        for (int remove_inserted_customer = 0; remove_inserted_customer < IRPSolution.UnallocatedCustomers[move[3]].size(); remove_inserted_customer++)
        {
            if (IRPSolution.UnallocatedCustomers[move[3]][remove_inserted_customer] == move[8])
            {
                whether_remove_inserted_customer_exists = true;
                IRPSolution.UnallocatedCustomers[move[3]].erase(IRPSolution.UnallocatedCustomers[move[3]].begin()+ remove_inserted_customer, IRPSolution.UnallocatedCustomers[move[3]].begin()+ remove_inserted_customer + move[6]);
                break;
            }
        }
        assert(whether_remove_inserted_customer_exists == true);
        memory.TrackSolutionStatus[move[0]][move[1]] = 1;          // Mark route as changed
        memory.TrackSolutionStatus[move[3]][move[4]] = 1;          // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[0]][move[1]] = 1; // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[3]][move[4]] = 1; // Mark route as changed
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
        // // IRPSolution.Validation(IRPLR);
        // assert(fabs(ImpTotalDelivery - CheckTotalDelivery) < 0.00001);
    }
    return whether_improved_or_not;
}