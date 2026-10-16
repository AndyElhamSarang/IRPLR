#include "lib.h"

int solution_improvement::OperatorTransfer(
    input &IRPLR,
    solution &IRPSolution,
    double &PenaltyForStockOut,
    double &PenaltyMoreThanCapacity,
    vector<vector<int>> &TransferDetails,
    preprocessing &memory)
{ // Current implementation only permits at most one customer therefore

    cout << "================================" << endl;
    cout << "OperatorTransfer" << endl;
    int TransferNumber = 1;
    assert(TransferNumber <= 1);

    int whether_improved_or_not = 0;
    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, IRPSolution.ViolationMoreThanCapacity);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << "\t ViolationMoreThanCapacity:" << IRPSolution.ViolationMoreThanCapacity << "\t PenaltyMoreThanCapacity:" << PenaltyMoreThanCapacity << "\t LR_objv:" << LR_objv << endl;

    // cout << "LR objv:" << LR_objv << endl;
    double objv_begin = LR_objv;
    vector<double> ImpDeliveryQuantityCustomerTransfer;
    vector<double> ImpInventoryLevelCustomerTransfer;
    bool IntroduceNewQuantityOrNotTransfer = false;

    vector<vector<double>> ImpVehicleLoad;
    double ImpStockOut = 0;
    double ImpVehicleOverload = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;

    // memory.PopulateQuantityStructure(IRPLR, IRPSolution);
    vector<int> move;
    for (int i = 0; i < 9; i++)
    {
        move.push_back(0);
        // move[0]: day_remove
        // move[1]: vehicle_remove
        // move[2]: position_in_vehicle_remove
        // move[3]: day_insert
        // move[4]: vehicle_insert
        // move[5]: position_in_vehicle_insert
        // move[6]: length
        // move[7]: dummy_0
        // move[8]: customer_index
    }

    int solutionCounter = 0;
    int working_solutionCounter = 0;
    int not_working_solutionCounter = 0;
    int naive_implementation = 0;

    time(&accumulate_start_time);
    try
    {

        for (int i = 0; i < TransferDetails.size(); i++)
        {
            int customer_idx = TransferDetails[i][0];
            int pick_day_remove = TransferDetails[i][1];
            int pick_day_insert = TransferDetails[i][2];

            time(&total_end_time);
            double total_ls_time = difftime(total_end_time, total_start_time);
            assert(total_ls_time - AccumulatedTimeHGS >= 0);
            if (total_ls_time - MainAlgorithmTimeLimit - AccumulatedTimeHGS > -0.01)
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
            bool Whether_normal_insert_fail = false; // if insert does not lead to an increase on delivery quantity, this move fails, therefore skip the evalution

            /////////////////////////////////////////////////////////////////////////////////
            //                                                                             //
            //      Evaluate quantity changes without permitting capacity violation        //
            //                                                                             //
            /////////////////////////////////////////////////////////////////////////////////
            // Initialize all data needed to capture the improving solution
            vector<double> NewDeliveryQuantityCustomerTransfer(IRPSolution.DeliveryQuantity[customer_idx]);
            vector<double> NewInventoryLevelCustomerTransfer(IRPSolution.InventoryLevel[customer_idx]);
            bool NewIntroduceNewQuantityOrNotTransfer = false;
            double NewStockOut = IRPSolution.ViolationStockOut;
            double NewVehicleOverload = IRPSolution.ViolationMoreThanCapacity;
            double ChangeInTotalQuantity = 0;
            vector<vector<double>> CopyVehicleLoad = IRPSolution.VehicleLoad;
            vector<double> CopyTotalSlackPerVehicle = memory.TotalSlackPerVehicle[pick_day_insert];
            // cout << "================================================================" << endl;
            // cout << "TransferDetails: Retailer " << customer_idx << ", from day " << pick_day_remove << " to day " << pick_day_insert << endl;
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

            CopyVehicleLoad[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]] = CopyVehicleLoad[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]] - IRPSolution.DeliveryQuantity[customer_idx][pick_day_remove];
            NewDeliveryQuantityCustomerTransfer[pick_day_remove] = 0;
            ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[customer_idx][pick_day_remove];

            // The following code adjust the delivery quantity and inventory level of the removed customer across all affected periods.
            if (pick_day_remove == 0)
            {
                AdjustQuantityAndInventoryLevel(
                    IRPLR.Retailers[customer_idx].InventoryBegin,
                    pick_day_remove,
                    IRPSolution.VehicleAllocation[customer_idx][pick_day_remove],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    NewVehicleOverload,
                    customer_idx,
                    IRPLR);
            }
            else
            {
                AdjustQuantityAndInventoryLevel(
                    IRPSolution.InventoryLevel[customer_idx][pick_day_remove - 1],
                    pick_day_remove,
                    IRPSolution.VehicleAllocation[customer_idx][pick_day_remove],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    NewVehicleOverload,
                    customer_idx,
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

            ////////////////////////////////////////////////////////////////////////////////////
            //                                                                                //
            //                Insert to a vehicle with largest delivery quantity              //
            //                                                                                //
            ////////////////////////////////////////////////////////////////////////////////////

            NewDeliveryQuantityCustomerTransfer[pick_day_insert] = 0;
            int VehicleIndexInserted = IRPLR.NumberOfVehicles + 1;
            for (int VehicleIndexNewDay = 0; VehicleIndexNewDay < IRPLR.NumberOfVehicles; VehicleIndexNewDay++)
            {
                double temp_NewDeliveryQuantityCustomerTransfer = 0;
                if (pick_day_insert == 0)
                {
                    temp_NewDeliveryQuantityCustomerTransfer = DeliveryMax(
                        IRPLR.Vehicle.capacity,
                        CopyVehicleLoad[pick_day_insert][VehicleIndexNewDay],
                        IRPLR.Retailers[customer_idx].InventoryMax,
                        IRPLR.Retailers[customer_idx].Demand,
                        IRPLR.Retailers[customer_idx].InventoryBegin);
                }
                else
                {
                    temp_NewDeliveryQuantityCustomerTransfer = DeliveryMax(
                        IRPLR.Vehicle.capacity,
                        CopyVehicleLoad[pick_day_insert][VehicleIndexNewDay],
                        IRPLR.Retailers[customer_idx].InventoryMax,
                        IRPLR.Retailers[customer_idx].Demand,
                        NewInventoryLevelCustomerTransfer[pick_day_insert - 1]);
                }
                if (temp_NewDeliveryQuantityCustomerTransfer > NewDeliveryQuantityCustomerTransfer[pick_day_insert])
                {
                    NewDeliveryQuantityCustomerTransfer[pick_day_insert] = temp_NewDeliveryQuantityCustomerTransfer;
                    VehicleIndexInserted = VehicleIndexNewDay;
                }
            }

            if (NewDeliveryQuantityCustomerTransfer[pick_day_insert] < 0.00001)
            {
                NewDeliveryQuantityCustomerTransfer[pick_day_insert] = 0;
                VehicleIndexInserted = IRPLR.NumberOfVehicles + 1;
                if (pick_day_insert == 0)
                {
                    NewDeliveryQuantityCustomerTransfer[pick_day_insert] = DeliveryMaxAllowingCapacityViolation(
                        IRPLR.Retailers[customer_idx].InventoryMax,
                        IRPLR.Retailers[customer_idx].Demand,
                        IRPLR.Retailers[customer_idx].InventoryBegin);
                }
                else
                {
                    NewDeliveryQuantityCustomerTransfer[pick_day_insert] = DeliveryMaxAllowingCapacityViolation(
                        IRPLR.Retailers[customer_idx].InventoryMax,
                        IRPLR.Retailers[customer_idx].Demand,
                        NewInventoryLevelCustomerTransfer[pick_day_insert - 1]);
                }
                double temp_largest_quantity = 0;
                int temp_largest_quantity_vehicle = IRPLR.NumberOfVehicles + 1;
                for (int temp_vehicle = 0; temp_vehicle < CopyTotalSlackPerVehicle.size(); temp_vehicle++)
                {

                    double temp_NewDeliveryQuantityCustomerTransfer = min(NewDeliveryQuantityCustomerTransfer[pick_day_insert], CopyTotalSlackPerVehicle[temp_vehicle]);
                    if (temp_largest_quantity < temp_NewDeliveryQuantityCustomerTransfer)
                    {
                        temp_largest_quantity = temp_NewDeliveryQuantityCustomerTransfer;
                        temp_largest_quantity_vehicle = temp_vehicle;
                    }
                    // cout << "Customer:" << customer_idx << " from day " << pick_day_remove << " to day " << pick_day_insert << ", Delivery Max Allowing Capacity Violation: " << NewDeliveryQuantityCustomerTransfer[pick_day_insert] << ", CopyTotalSlackPerVehicle[temp_vehicle]: " << CopyTotalSlackPerVehicle[temp_vehicle] << ", Delivery Quantity can be added:" << min(NewDeliveryQuantityCustomerTransfer[pick_day_insert], CopyTotalSlackPerVehicle[temp_vehicle]) << endl;
                }
                // cout << "Quantity to add:" << min(NewDeliveryQuantityCustomerTransfer[pick_day_insert], temp_largest_quantity) << " to vehicle " << temp_largest_quantity_vehicle << " new slack " << CopyTotalSlackPerVehicle[temp_largest_quantity_vehicle] - NewDeliveryQuantityCustomerTransfer[pick_day_insert] << endl;

                NewDeliveryQuantityCustomerTransfer[pick_day_insert] = min(NewDeliveryQuantityCustomerTransfer[pick_day_insert], temp_largest_quantity);
                VehicleIndexInserted = temp_largest_quantity_vehicle;
                CopyTotalSlackPerVehicle[VehicleIndexInserted] = CopyTotalSlackPerVehicle[VehicleIndexInserted] - NewDeliveryQuantityCustomerTransfer[pick_day_insert];
                NewIntroduceNewQuantityOrNotTransfer = true;
            }

            if (NewDeliveryQuantityCustomerTransfer[pick_day_insert] < 0.00001)
            {
                Whether_normal_insert_fail = true;
                assert(VehicleIndexInserted == IRPLR.NumberOfVehicles + 1);
            }
            else
            {
                assert(NewDeliveryQuantityCustomerTransfer[pick_day_insert] > 0.00001);
                assert(IRPSolution.DeliveryQuantity[customer_idx][pick_day_insert] < 0.00001);
                if (NewIntroduceNewQuantityOrNotTransfer == false)
                {
                    ChangeInTotalQuantity += NewDeliveryQuantityCustomerTransfer[pick_day_insert] - IRPSolution.DeliveryQuantity[customer_idx][pick_day_insert];
                    CopyVehicleLoad[pick_day_insert][VehicleIndexInserted] = CopyVehicleLoad[pick_day_insert][VehicleIndexInserted] + NewDeliveryQuantityCustomerTransfer[pick_day_insert];
                }
                if (pick_day_insert == 0)
                {
                    AdjustQuantityAndInventoryLevel(
                        IRPLR.Retailers[customer_idx].InventoryBegin,
                        pick_day_insert,
                        VehicleIndexInserted,
                        NewDeliveryQuantityCustomerTransfer,
                        NewInventoryLevelCustomerTransfer,
                        CopyVehicleLoad,
                        IRPSolution.VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        NewVehicleOverload,
                        customer_idx,
                        IRPLR);
                }
                else
                {
                    AdjustQuantityAndInventoryLevel(
                        NewInventoryLevelCustomerTransfer[pick_day_insert - 1],
                        pick_day_insert,
                        VehicleIndexInserted,
                        NewDeliveryQuantityCustomerTransfer,
                        NewInventoryLevelCustomerTransfer,
                        CopyVehicleLoad,
                        IRPSolution.VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        NewVehicleOverload,
                        customer_idx,
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

            if (Whether_normal_insert_fail == false) // If both insert fails, then we do not need to calculate the objective value, because it is infeasible
            {
                working_solutionCounter++;
                int temp_int = IRPLR.NumberOfVehicles + 1;
                int temp_int2 = 0;
                double NewRouteCostRemoved = memory.ConcatenateSwapTwoRoutesSingleDay(
                    pick_day_remove,
                    IRPSolution.VehicleAllocation[customer_idx][pick_day_remove],
                    IRPSolution.VisitOrder[customer_idx][pick_day_remove],
                    TransferNumber,
                    temp_int,
                    temp_int2,
                    temp_int2,
                    IRPSolution,
                    IRPLR,
                    memory);
                int CheapestInsertPosition = IRPSolution.Route[pick_day_insert][VehicleIndexInserted].size() + 1;
                double NewRouteCostInserted = numeric_limits<double>::max();
                for (int insert_position = 0; insert_position <= IRPSolution.Route[pick_day_insert][VehicleIndexInserted].size(); insert_position++) // A position in the new vehicle
                {

                    double TempNewRouteCostInserted = memory.ConcatenateChepestInsertion(
                        pick_day_insert,
                        VehicleIndexInserted,
                        insert_position,
                        temp_int2,
                        pick_day_remove,
                        IRPSolution.VehicleAllocation[customer_idx][pick_day_remove],
                        IRPSolution.VisitOrder[customer_idx][pick_day_remove],
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
                assert(CheapestInsertPosition < IRPSolution.Route[pick_day_insert][VehicleIndexInserted].size() + 1);

                double NewTotalTransportationCost =
                    IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]] + NewRouteCostRemoved - IRPSolution.TransportationCostPerRoute[pick_day_insert][VehicleIndexInserted] + NewRouteCostInserted;

                // cout << "PenaltyForStockOut: " << PenaltyForStockOut << endl;
                // cout << "PenaltyMoreThanCapacity: " << PenaltyMoreThanCapacity << endl;

                double NewTotalDelivery = std::numeric_limits<double>::max();
                double NewLogisticRatio = std::numeric_limits<double>::max();
                double temp_LR_objv = std::numeric_limits<double>::max();

                if (Whether_normal_insert_fail == false)
                {
                    NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;
                    NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                    temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, NewStockOut, PenaltyMoreThanCapacity, NewVehicleOverload);

                    // cout << "Whether_normal_insert_fail: " << Whether_normal_insert_fail << endl;
                    // cout << "NewTotalTransportationCost: " << NewTotalTransportationCost << endl;
                    // cout << "NewTotalDelivery: " << NewTotalDelivery << endl;
                    // cout << "NewLogisticRatio: " << NewLogisticRatio << endl;
                    // cout << "NewStockOut: " << NewStockOut << endl;
                    // cout << "NewVehicleOverload: " << NewVehicleOverload << endl;
                    // cout << "temp_LR_objv: " << temp_LR_objv << endl;
                }
                //  cout<<"objv_begin:"<<objv_begin<<" temp_LR_objv:" << temp_LR_objv << endl;
                if (objv_begin - temp_LR_objv > 0.00001)
                {

                    if (LR_objv - temp_LR_objv > 0.00001)
                    {

                        whether_improved_or_not = 1;
                        LR_objv = temp_LR_objv;

                        move[0] = pick_day_remove;                                              // pick_day1
                        move[1] = IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]; // pick_vehicle1
                        move[2] = IRPSolution.VisitOrder[customer_idx][pick_day_remove];        // pick_position_in_vehicle1
                        move[3] = pick_day_insert;                                              // pick_day2
                        move[4] = VehicleIndexInserted;                                         // pick_vehicle2
                        move[5] = CheapestInsertPosition;                                       // pick_position_in_vehicle2
                        move[6] = TransferNumber;                                               // Swap_length1
                        move[7] = 0;                                                            // Swap_length2
                        move[8] = customer_idx;                                                 // customer_index
                        ImpDeliveryQuantityCustomerTransfer.clear();
                        ImpInventoryLevelCustomerTransfer.clear();
                        ImpDeliveryQuantityCustomerTransfer = NewDeliveryQuantityCustomerTransfer;
                        ImpInventoryLevelCustomerTransfer = NewInventoryLevelCustomerTransfer;
                        ImpVehicleLoad = CopyVehicleLoad;
                        ImpStockOut = NewStockOut;
                        ImpLogisticRatio = NewLogisticRatio;
                        ImpTotalTransportationCost = NewTotalTransportationCost;
                        ImpTotalDelivery = NewTotalDelivery;
                        IntroduceNewQuantityOrNotTransfer = NewIntroduceNewQuantityOrNotTransfer;
                    }
                }

                ////////////////////////////////////////////////////////////////////////
                //                                                                    //
                //      Verify the correctness of the efficient implementation        //
                //                                                                    //
                ////////////////////////////////////////////////////////////////////////

                // vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                // cout << "Customer " << customer_idx << ", "
                //      << "remove_day: " << pick_day_remove << ", remove from vehicle: " << IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]
                //      << ", remove position: " << IRPSolution.VisitOrder[customer_idx][pick_day_remove] << ", remove_length: " << TransferNumber
                //      << ", insert_day: " << pick_day_insert << ", insert vehicle: " << VehicleIndexInserted << ", insert_position: " << CheapestInsertPosition << ", insert_length: " << TransferNumber << endl;
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
                // TempRoute[pick_day_insert][VehicleIndexInserted].insert(TempRoute[pick_day_insert][VehicleIndexInserted].begin() + CheapestInsertPosition,
                //                                                               TempRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]].begin() + IRPSolution.VisitOrder[customer_idx][pick_day_remove],
                //                                                               TempRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]].begin() + IRPSolution.VisitOrder[customer_idx][pick_day_remove] + TransferNumber);
                // TempRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]].erase(
                //     TempRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]].begin() + IRPSolution.VisitOrder[customer_idx][pick_day_remove],
                //     TempRoute[pick_day_remove][IRPSolution.VehicleAllocation[customer_idx][pick_day_remove]].begin() + IRPSolution.VisitOrder[customer_idx][pick_day_remove] + TransferNumber);

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
                not_working_solutionCounter++;
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
    cout << "working_solutionCounter:" << working_solutionCounter << ";";
    cout << "Not working solutions:" << not_working_solutionCounter << ";";
    cout << "whether_improved_or_not:" << whether_improved_or_not << ";";
    cout << "ImpLogisticRatio:" << ImpLogisticRatio << ", ImpStockOut:" << ImpStockOut << endl;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //       Make up the solution, adjust its inventory level and route       //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    if (whether_improved_or_not == 1) // whether_improved_or_not==1
    {
        // IRPSolution.print_solution(IRPLR);

        cout << "day_remove:" << move[0]
             << ", vehicle_remove:" << move[1]
             << ", position_in_vehicle_remove:" << move[2]
             << ", day_insert:" << move[3]
             << ", vehicle_insert:" << move[4]
             << ", position_in_vehicle_insert:" << move[5]
             << ", length:" << move[6]
             << ", dummy_0:" << move[7]
             << ", customer_index" << move[8] << endl;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;
        IRPSolution.ViolationMoreThanCapacity = ImpVehicleOverload;
        IRPSolution.TotalTransportationCost = ImpTotalTransportationCost;
        IRPSolution.TotalDelivery = ImpTotalDelivery;
        IRPSolution.DeliveryQuantity[move[8]] = ImpDeliveryQuantityCustomerTransfer;
        IRPSolution.InventoryLevel[move[8]] = ImpInventoryLevelCustomerTransfer;

        // for (int i = 0; i < IRPSolution.Route.size(); i++)
        // {
        //     for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        //     {
        //         if (IRPSolution.Route[i][j].size() != 0)
        //         {
        //             cout << "Route for day " << i << ", vehicle " << j << ": ";
        //             for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
        //             {
        //                 cout << IRPSolution.Route[i][j][k] << ", ";
        //             }
        //             cout << endl;
        //         }
        //     }
        //     cout << "Unallocated customer for day " << i << ": ";
        //     for (int j = 0; j < IRPSolution.UnallocatedCustomers[i].size(); j++)
        //     {
        //         cout << IRPSolution.UnallocatedCustomers[i][j] << ",";
        //     }
        //     cout<< endl;
        // }

        // Customer in Day1 becomes unallocated
        IRPSolution.UnallocatedCustomers[move[0]].insert(IRPSolution.UnallocatedCustomers[move[0]].begin(),
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                         IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        // This customer is visited in Day2 instead of Day1
        IRPSolution.Route[move[3]][move[4]].insert(IRPSolution.Route[move[3]][move[4]].begin() + move[5],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                   IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        // Remove the customer from the original route
        IRPSolution.Route[move[0]][move[1]].erase(IRPSolution.Route[move[0]][move[1]].begin() + move[2],
                                                  IRPSolution.Route[move[0]][move[1]].begin() + move[2] + move[6]);
        // Find this customer in the unallocated list of Day2, and remove it from there
        bool whether_remove_inserted_customer_exists = false;
        for (int remove_inserted_customer = 0; remove_inserted_customer < IRPSolution.UnallocatedCustomers[move[3]].size(); remove_inserted_customer++)
        {
            if (IRPSolution.UnallocatedCustomers[move[3]][remove_inserted_customer] == move[8])
            {

                whether_remove_inserted_customer_exists = true;
                IRPSolution.UnallocatedCustomers[move[3]].erase(IRPSolution.UnallocatedCustomers[move[3]].begin() + remove_inserted_customer, IRPSolution.UnallocatedCustomers[move[3]].begin() + remove_inserted_customer + move[6]);
                break;
            }
        }

        assert(whether_remove_inserted_customer_exists == true);
        memory.TrackSolutionStatus[move[0]][move[1]] = 1;          // Mark route as changed
        memory.TrackSolutionStatus[move[3]][move[4]] = 1;          // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[0]][move[1]] = 1; // Mark route as changed
        memory.TrackSingleRouteOptimisation[move[3]][move[4]] = 1; // Mark route as changed
        IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);

        IRPSolution.print_solution(IRPLR);
        boost_random_mechanism RandomRemoval;
        double QuantityToBeRemoved = ImpDeliveryQuantityCustomerTransfer[move[3]];
        cout << "QuantityToBeRemoved:" << QuantityToBeRemoved <<", IntroduceNewQuantityOrNotTransfer"<<IntroduceNewQuantityOrNotTransfer<< endl;
        if (IntroduceNewQuantityOrNotTransfer == true)
        {
            int WhileLoopCounter = 0;
            int vehicle_max_length = IRPSolution.Route[move[3]][move[4]].size();
            while (QuantityToBeRemoved > 0)
            {
                double selected_quantity = 0;
                int selected_quantity_customer = IRPLR.Retailers.size() + 1;
                vector<double> QuantityCandidates;
                vector<int> CustomerCandidates;
                for (int customer = 0; customer < memory.QuantityStructure.size(); customer++)
                {
                    if (IRPSolution.VehicleAllocation[customer][move[3]] == move[4] && memory.QuantityStructure[customer][move[3]] > 0)
                    {
                        bool AddCandidate = true;
                        // cout << "Customer inserted:"<<move[8]<<endl;
                        if (customer == move[8])
                        {
                            AddCandidate = false;
                        }
                        // cout << endl;
                        if (AddCandidate)
                        {
                            QuantityCandidates.push_back(memory.QuantityStructure[customer][move[3]]);
                            CustomerCandidates.push_back(customer);
                        }
                    }
                }
                for (int test = 0; test < QuantityCandidates.size(); test++)
                {
                    cout << QuantityCandidates[test] << "," << CustomerCandidates[test] << endl;
                }
                assert(QuantityCandidates.size() > 0);
                int SelectedIndex = RandomRemoval.random_number_generator(0, QuantityCandidates.size() - 1, generator);
                selected_quantity = QuantityCandidates[SelectedIndex];
                selected_quantity_customer = CustomerCandidates[SelectedIndex];

                cout << "Selected:" << selected_quantity << "," << selected_quantity_customer << endl;

                IRPSolution.DeliveryQuantity[selected_quantity_customer][move[3]] -= min(QuantityToBeRemoved, selected_quantity);
                assert(IRPSolution.VehicleAllocation[selected_quantity_customer][move[3]] < IRPLR.NumberOfVehicles);
                double TempChangeInTotalQuantity = 0.0; // Initialize appropriately
                double TempStockOut = 0.0;              // Initialize appropriately
                double TempVehicleOverload = 0.0;       // Initialize appropriately
                AdjustQuantityAndInventoryLevel(
                    IRPSolution.InventoryLevel[selected_quantity_customer][move[3] - 1],
                    move[3],
                    move[4],
                    IRPSolution.DeliveryQuantity[selected_quantity_customer],
                    IRPSolution.InventoryLevel[selected_quantity_customer],
                    IRPSolution.VehicleLoad,
                    IRPSolution.VehicleAllocation,
                    TempChangeInTotalQuantity,
                    TempStockOut,
                    TempVehicleOverload,
                    selected_quantity_customer,
                    IRPLR);

                memory.UpdateQuantityStructure(IRPLR, IRPSolution, selected_quantity_customer, move[3], IRPSolution.VehicleAllocation[selected_quantity_customer][move[3]]);
                QuantityToBeRemoved -= min(QuantityToBeRemoved, selected_quantity);

                WhileLoopCounter++;
                assert(WhileLoopCounter <= vehicle_max_length && "While loop for removing quantity is running too long");
            }
        }
        bool cleaned = false;
        while (cleaned == false)
        {
            cleaned = true;
            for (int customer = 0; customer < IRPSolution.Route[move[3]][move[4]].size(); customer++)
            {
                if (IRPSolution.DeliveryQuantity[IRPSolution.Route[move[3]][move[4]][customer]][move[3]] < 0.00001)
                {

                    cleaned = false;
                    int customer_candidate = IRPSolution.Route[move[3]][move[4]][customer];
                    IRPSolution.UnallocatedCustomers[move[3]].push_back(customer_candidate);
                    IRPSolution.Route[move[3]][move[4]].erase(IRPSolution.Route[move[3]][move[4]].begin() + customer,
                                                              IRPSolution.Route[move[3]][move[4]].begin() + customer + 1);

                    for (int next_customer = customer; next_customer < IRPSolution.Route[move[3]][move[4]].size(); next_customer++)
                    {
                        IRPSolution.VisitOrder[IRPSolution.Route[move[3]][move[4]][next_customer]][move[3]] -= 1;
                    }
                    IRPSolution.VisitOrder[customer_candidate][move[3]] = IRPLR.Retailers.size() + 1;
                    IRPSolution.VehicleAllocation[customer_candidate][move[3]] = IRPLR.NumberOfVehicles + 1;
                    break;
                }
            }
        }
        IRPSolution.print_solution(IRPLR);
        // if(IntroduceNewQuantityOrNotTransfer == true)
        // {
        //     bool flag = false;
        //     assert(flag);
        // }
        // ////////////////////////////////////////////////////////////////////////
        // //                                                                    //
        // //             Verify the correctness of the output                   //
        // //                                                                    //
        // ////////////////////////////////////////////////////////////////////////

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

    cout << "================================" << endl;
    return whether_improved_or_not;
}