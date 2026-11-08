#include "lib.h"

int solution_improvement::OperatorSwapTwoRoutesOnSingleDay(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, double &PenaltyMoreThanCapacity, preprocessing &memory, set<vector<int>> &SwapTwoRoutesOnSingleDayPair, set<vector<int>> &SwapTwoRoutesOnSingleDayPairToReconsider,
                                                           int &min_Swap_length1, int &max_Swap_length1,
                                                           int &min_Swap_length2, int &max_Swap_length2)
{
    cout << "================================" << endl;
    cout << "OperatorSwapTwoRoutesOnSingleDay" << endl;
    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    memory.PopulateQuantityStructure(IRPLR, IRPSolution);
    // IRPSolution.print_solution(IRPLR);
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
    for (int i = 0; i < 7; i++)
    {
        move.push_back(0);
        // move[0]: pick_day
        // move[1]: pick_vehicle1
        // move[2]: pick_position_in_vehicle1
        // move[3]: pick_vehicle2
        // move[4]: pick_position_in_vehicle2
        // move[5]: Swap_length1
        // move[6]: Swap_length2
    }
    int whether_improved_or_not = 0;

    vector<vector<double>> ImpDeliveryQuantityRoute1;
    vector<vector<double>> ImpInventoryLevelRoute1;
    vector<bool> IntroduceNewQuantityOrNotRoute1;
    vector<vector<double>> ImpDeliveryQuantityRoute2;
    vector<vector<double>> ImpInventoryLevelRoute2;
    vector<bool> IntroduceNewQuantityOrNotRoute2;
    vector<vector<double>> ImpVehicleLoad;
    vector<double> ImpTotalSlackPerVehicle;
    double ImpStockOut = 0;
    double ImpVehicleOverload = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;
    int solutionCounter = 0;
    int working_solutionCounter = 0;
    int not_working_solutionCounter = 0;
    set<vector<int>>::iterator select_pair;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                                        //
    //  Evaluation caches.                                                                                    //
    //  The quantity part of a move only depends on WHICH customers change vehicle, not on the positions      //
    //  where they are inserted. Therefore:                                                                   //
    //   (1) moves with Swap_length2 == 0 (e.g. the shift phase) are evaluated once per                       //
    //       (pick_position_in_vehicle1, Swap_length1) and reused for every pick_position_in_vehicle2;        //
    //   (2) moves with Swap_length1 == 0 are evaluated once per (pick_position_in_vehicle2, Swap_length2)    //
    //       and reused for every pick_position_in_vehicle1.                                                  //
    //  Only the routing cost (ConcatenateSwapTwoRoutesSingleDay) is evaluated for every position. The        //
    //  arithmetic is exactly the one of evaluating each candidate from scratch, so the search is unchanged.  //
    //  All buffers are allocated once per operator call and reused, avoiding per-candidate heap traffic.     //
    //                                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct SwapEvaluation
    {
        bool computed = false;
        bool fail_insert = false;   // an inserted customer cannot receive a positive quantity
        bool fail_supplier = false; // the final supplier availability check fails
        vector<vector<double>> DeliveryQuantityRoute1, InventoryLevelRoute1;
        vector<vector<double>> DeliveryQuantityRoute2, InventoryLevelRoute2;
        vector<char> IntroduceNewQuantityRoute1, IntroduceNewQuantityRoute2;
        vector<vector<double>> VehicleLoad;      // [day][vehicle]
        vector<double> TotalSlackPerVehicle;     // [vehicle] on pick_day
        double ChangeInTotalQuantity = 0;
        double StockOut = 0;
        double VehicleOverload = 0;
    };
    const int MaxRows1 = max(max_Swap_length1, 0);
    const int MaxRows2 = max(max_Swap_length2, 0);
    SwapEvaluation ScratchEvaluation;
    vector<SwapEvaluation> CacheFixedPosition1(MaxRows1 + 1); // Swap_length2 == 0: indexed by Swap_length1, valid for the current pick_position_in_vehicle1
    vector<SwapEvaluation> CacheFixedPosition2;               // Swap_length1 == 0: indexed by pick_position_in_vehicle2 * (MaxRows2 + 1) + Swap_length2, valid for the current pair
    vector<int> CandidateCustomers(MaxRows1 + MaxRows2);
    vector<const double *> CandidateRows(MaxRows1 + MaxRows2);

    auto EvaluateSwap = [&](int pick_day, int pick_vehicle1, int pick_position_in_vehicle1, int Swap_length1,
                            int pick_vehicle2, int pick_position_in_vehicle2, int Swap_length2, SwapEvaluation &out)
    {
        if ((int)out.DeliveryQuantityRoute1.size() < Swap_length1)
        {
            out.DeliveryQuantityRoute1.resize(Swap_length1);
            out.InventoryLevelRoute1.resize(Swap_length1);
            out.IntroduceNewQuantityRoute1.resize(Swap_length1);
        }
        if ((int)out.DeliveryQuantityRoute2.size() < Swap_length2)
        {
            out.DeliveryQuantityRoute2.resize(Swap_length2);
            out.InventoryLevelRoute2.resize(Swap_length2);
            out.IntroduceNewQuantityRoute2.resize(Swap_length2);
        }
        out.fail_insert = false;
        out.fail_supplier = false;

        double SlackDeductRoute1 = 0;
        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
        {
            const int customer = IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index];
            out.DeliveryQuantityRoute1[route1_index] = IRPSolution.DeliveryQuantity[customer];
            out.InventoryLevelRoute1[route1_index] = IRPSolution.InventoryLevel[customer];
            out.IntroduceNewQuantityRoute1[route1_index] = false;
            SlackDeductRoute1 += memory.QuantityStructure[customer][pick_day];
        }
        double SlackDeductRoute2 = 0;
        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
        {
            const int customer = IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index];
            out.DeliveryQuantityRoute2[route2_index] = IRPSolution.DeliveryQuantity[customer];
            out.InventoryLevelRoute2[route2_index] = IRPSolution.InventoryLevel[customer];
            out.IntroduceNewQuantityRoute2[route2_index] = false;
            SlackDeductRoute2 += memory.QuantityStructure[customer][pick_day];
        }
        // Candidates for the supplier availability check: route 1 customers first, then route 2 customers
        int NumberOfCandidates = 0;
        for (int route1_index = 0; route1_index < Swap_length1; ++route1_index)
        {
            CandidateCustomers[NumberOfCandidates] = IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index];
            out.DeliveryQuantityRoute1[route1_index][pick_day] = 0.0;
            CandidateRows[NumberOfCandidates] = out.DeliveryQuantityRoute1[route1_index].data();
            NumberOfCandidates++;
        }
        for (int route2_index = 0; route2_index < Swap_length2; ++route2_index)
        {
            CandidateCustomers[NumberOfCandidates] = IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index];
            out.DeliveryQuantityRoute2[route2_index][pick_day] = 0.0;
            CandidateRows[NumberOfCandidates] = out.DeliveryQuantityRoute2[route2_index].data();
            NumberOfCandidates++;
        }

        out.StockOut = IRPSolution.ViolationStockOut;
        out.VehicleOverload = IRPSolution.ViolationMoreThanCapacity;
        out.ChangeInTotalQuantity = 0;
        out.VehicleLoad = IRPSolution.VehicleLoad;
        out.TotalSlackPerVehicle = memory.TotalSlackPerVehicle[pick_day];

        // Remove customers from route 1 and route 2, make delivery quantity 0 for removed customers and record the change in total delivery quantity
        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
        {
            const int customer = IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index];
            out.VehicleLoad[pick_day][pick_vehicle1] = out.VehicleLoad[pick_day][pick_vehicle1] - IRPSolution.DeliveryQuantity[customer][pick_day];
            out.DeliveryQuantityRoute1[route1_index][pick_day] = 0;
            out.ChangeInTotalQuantity = out.ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[customer][pick_day];
        }
        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
        {
            const int customer = IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index];
            out.VehicleLoad[pick_day][pick_vehicle2] = out.VehicleLoad[pick_day][pick_vehicle2] - IRPSolution.DeliveryQuantity[customer][pick_day];
            out.DeliveryQuantityRoute2[route2_index][pick_day] = 0;
            out.ChangeInTotalQuantity = out.ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[customer][pick_day];
        }

        // Insert customers of route 1 into vehicle 2 (insert_to_vehicle) and customers of route 2 into vehicle 1.
        // Supplier availability is evaluated with the current customer's delivery on pick_day still at 0.
        auto InsertCustomer = [&](int customer, vector<double> &Delivery, vector<double> &Inventory, char &IntroduceNewQuantity,
                                  int insert_to_vehicle, double SlackDeductInsertToVehicle)
        {
            const double SupplierAvailable = SupplierAvailabilityForCandidateDay(
                IRPLR, IRPSolution, pick_day, CandidateCustomers.data(), CandidateRows.data(), NumberOfCandidates);
            double &previous_inventory_level = (pick_day == 0) ? IRPLR.Retailers[customer].InventoryBegin : IRPSolution.InventoryLevel[customer][pick_day - 1];
            Delivery[pick_day] = DeliveryMax(
                IRPLR.Vehicle.capacity,
                out.VehicleLoad[pick_day][insert_to_vehicle],
                IRPLR.Retailers[customer].InventoryMax,
                IRPLR.Retailers[customer].Demand,
                previous_inventory_level);
            // Seek delivery quantity from possible quantity to remove from the existing visits,
            if (Delivery[pick_day] < 0.00001)
            {
                // Strategy 1
                Delivery[pick_day] = DeliveryMaxAllowingCapacityViolation(
                    IRPLR.Retailers[customer].InventoryMax,
                    IRPLR.Retailers[customer].Demand,
                    previous_inventory_level);
                Delivery[pick_day] = min(Delivery[pick_day], out.TotalSlackPerVehicle[insert_to_vehicle] - SlackDeductInsertToVehicle);
                out.TotalSlackPerVehicle[insert_to_vehicle] = out.TotalSlackPerVehicle[insert_to_vehicle] - Delivery[pick_day];
                IntroduceNewQuantity = true;
            }
            const double quantityBeforeSupplierLimit = Delivery[pick_day];
            Delivery[pick_day] = min(quantityBeforeSupplierLimit, max(0.0, SupplierAvailable));
            if (IntroduceNewQuantity)
            {
                out.TotalSlackPerVehicle[insert_to_vehicle] += quantityBeforeSupplierLimit - Delivery[pick_day];
            }

            if (Delivery[pick_day] < 0.00001)
            {
                out.fail_insert = true;
            }
            else
            {
                if (IntroduceNewQuantity == false)
                {
                    out.ChangeInTotalQuantity += Delivery[pick_day];
                    out.VehicleLoad[pick_day][insert_to_vehicle] = out.VehicleLoad[pick_day][insert_to_vehicle] + Delivery[pick_day];
                }
                assert(Delivery[pick_day] != 0);
                AdjustQuantityAndInventoryLevel(
                    previous_inventory_level,
                    pick_day,
                    insert_to_vehicle,
                    Delivery,
                    Inventory,
                    out.VehicleLoad,
                    IRPSolution.VehicleAllocation,
                    out.ChangeInTotalQuantity,
                    out.StockOut,
                    out.VehicleOverload,
                    customer,
                    IRPLR);
            }
        };
        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
        {
            InsertCustomer(IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index],
                           out.DeliveryQuantityRoute1[route1_index], out.InventoryLevelRoute1[route1_index], out.IntroduceNewQuantityRoute1[route1_index],
                           pick_vehicle2, SlackDeductRoute2);
        }
        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
        {
            InsertCustomer(IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index],
                           out.DeliveryQuantityRoute2[route2_index], out.InventoryLevelRoute2[route2_index], out.IntroduceNewQuantityRoute2[route2_index],
                           pick_vehicle1, SlackDeductRoute1);
        }

        if (out.fail_insert == false)
        {
            if (SupplierAvailabilityForCandidateDay(
                    IRPLR, IRPSolution, pick_day, CandidateCustomers.data(), CandidateRows.data(), NumberOfCandidates) < -0.00001)
            {
                out.fail_supplier = true;
            }
        }
    };

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
            const int Route1Size = IRPSolution.Route[pick_day][pick_vehicle1].size();
            const int Route2Size = IRPSolution.Route[pick_day][pick_vehicle2].size();

            const size_t CacheFixedPosition2Size = (size_t)(Route2Size + 1) * (MaxRows2 + 1);
            if (CacheFixedPosition2.size() < CacheFixedPosition2Size)
            {
                CacheFixedPosition2.resize(CacheFixedPosition2Size);
            }
            for (size_t i = 0; i < CacheFixedPosition2Size; i++)
            {
                CacheFixedPosition2[i].computed = false;
            }

            for (int pick_position_in_vehicle1 = 0; pick_position_in_vehicle1 <= Route1Size; pick_position_in_vehicle1++) // A position in vehicle 1
            {
                for (int i = 0; i <= MaxRows1; i++)
                {
                    CacheFixedPosition1[i].computed = false;
                }
                for (int pick_position_in_vehicle2 = 0; pick_position_in_vehicle2 <= Route2Size; pick_position_in_vehicle2++) // A position in vehicle 2
                {
                    for (int Swap_length1 = min_Swap_length1; Swap_length1 <= max_Swap_length1; Swap_length1++)
                    {
                        for (int Swap_length2 = min_Swap_length2; Swap_length2 <= max_Swap_length2; Swap_length2++)
                        {
                            if (Swap_length1 != 0 || Swap_length2 != 0)
                            {

                                if (pick_position_in_vehicle2 + Swap_length2 <= Route2Size && pick_position_in_vehicle1 + Swap_length1 <= Route1Size) // Make sure the operator does not go outside the range
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
                                        SwapEvaluation *eval = &ScratchEvaluation;
                                        if (Swap_length2 == 0)
                                        {
                                            eval = &CacheFixedPosition1[Swap_length1];
                                        }
                                        else if (Swap_length1 == 0)
                                        {
                                            eval = &CacheFixedPosition2[(size_t)pick_position_in_vehicle2 * (MaxRows2 + 1) + Swap_length2];
                                        }
                                        if (eval == &ScratchEvaluation || !eval->computed)
                                        {
                                            EvaluateSwap(pick_day, pick_vehicle1, pick_position_in_vehicle1, Swap_length1,
                                                         pick_vehicle2, pick_position_in_vehicle2, Swap_length2, *eval);
                                            eval->computed = true;
                                        }

                                        if (eval->fail_insert == false)
                                        {
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
                                            double NewTotalTransportationCost =
                                                IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle1] - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle2] + NewRoute1Cost + NewRoute2Cost;

                                            double NewTotalDelivery = std::numeric_limits<double>::max();
                                            double NewLogisticRatio = std::numeric_limits<double>::max();
                                            double temp_LR_objv = std::numeric_limits<double>::max();
                                            if (eval->fail_supplier == false)
                                            {
                                                NewTotalDelivery = IRPSolution.TotalDelivery + eval->ChangeInTotalQuantity;
                                                NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                                                temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, eval->StockOut, PenaltyMoreThanCapacity, eval->VehicleOverload);
                                            }

                                            if (objv_begin - temp_LR_objv > 0.00001)
                                            {
                                                pair_obtain_improving = 1;
                                                if (LR_objv - temp_LR_objv > 0.00001)
                                                {
                                                    LR_objv = temp_LR_objv;
                                                    move[0] = pick_day;
                                                    move[1] = pick_vehicle1;
                                                    move[2] = pick_position_in_vehicle1;
                                                    move[3] = pick_vehicle2;
                                                    move[4] = pick_position_in_vehicle2;
                                                    move[5] = Swap_length1;
                                                    move[6] = Swap_length2;
                                                    whether_improved_or_not = 1;

                                                    ImpDeliveryQuantityRoute1.clear();
                                                    ImpInventoryLevelRoute1.clear();
                                                    ImpDeliveryQuantityRoute2.clear();
                                                    ImpInventoryLevelRoute2.clear();
                                                    for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
                                                    {
                                                        ImpDeliveryQuantityRoute1.push_back(eval->DeliveryQuantityRoute1[route1_index]);
                                                        ImpInventoryLevelRoute1.push_back(eval->InventoryLevelRoute1[route1_index]);
                                                    }
                                                    for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
                                                    {
                                                        ImpDeliveryQuantityRoute2.push_back(eval->DeliveryQuantityRoute2[route2_index]);
                                                        ImpInventoryLevelRoute2.push_back(eval->InventoryLevelRoute2[route2_index]);
                                                    }
                                                    ImpVehicleLoad = eval->VehicleLoad;
                                                    ImpStockOut = eval->StockOut;
                                                    ImpVehicleOverload = eval->VehicleOverload;
                                                    ImpLogisticRatio = NewLogisticRatio;
                                                    ImpTotalTransportationCost = NewTotalTransportationCost;
                                                    ImpTotalDelivery = NewTotalDelivery;
                                                    IntroduceNewQuantityOrNotRoute1.assign(eval->IntroduceNewQuantityRoute1.begin(), eval->IntroduceNewQuantityRoute1.begin() + Swap_length1);
                                                    IntroduceNewQuantityOrNotRoute2.assign(eval->IntroduceNewQuantityRoute2.begin(), eval->IntroduceNewQuantityRoute2.begin() + Swap_length2);
                                                    ImpTotalSlackPerVehicle = eval->TotalSlackPerVehicle;
                                                }
                                            }
                                        }
                                        else
                                        { // If failed in evalute, this pair is kept for next iteration, treat it easy`
                                            not_working_solutionCounter++;
                                            pair_obtain_improving = 1;
                                        }
                                    }
                                    else
                                    {
                                        ////////////////////////////////////////////////////////////////////////////////
                                        //                                                                            //
                                        //      Naive implementation of evaluating the swap remove insert move        //
                                        //                                                                            //
                                        ////////////////////////////////////////////////////////////////////////////////
                                        assert(Swap_length1 <= 3 && "Current code path not supported yet with Swap_length1 > 3");
                                        assert(Swap_length2 <= 3 && "Current code path not supported yet with Swap_length2 > 3");
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

        cout << "pick_day:" << move[0] << ", pick_vehicle1:" << move[1] << ", pick_position_in_vehicle1:" << move[2] << ", pick_vehicle2:" << move[3] << ", pick_position_in_vehicle2:" << move[4] << ", Swap_length1:" << move[5] << ", Swap_length2:" << move[6] << endl;

        // for (int i = 0; i < memory.TotalSlackPerVehicle.size(); i++)
        // {
        //     for (int j = 0; j < memory.TotalSlackPerVehicle[i].size(); j++)
        //     {
        //         cout << "Total slack for vehicle " << j << " on day " << i << ": " << memory.TotalSlackPerVehicle[i][j] << endl;
        //     }
        // }

        // cout << "Status of QuantityStructure(Slack):" << endl;
        // for (int i = 0; i < memory.QuantityStructure.size(); i++)
        // {
        //     for (int j = 0; j < memory.QuantityStructure[i].size(); j++)
        //     {
        //         cout << memory.QuantityStructure[i][j] << ",";
        //     }
        //     cout << endl;
        // }
        // for (int i = 0; i < memory.TotalSlackPerDay.size(); i++)
        // {
        //     cout << "Total slack for day " << i << ": " << memory.TotalSlackPerDay[i] << endl;
        // }
        bool LocalRebalanceTrigger = false;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;
        memory.TotalSlackPerVehicle[move[0]] = ImpTotalSlackPerVehicle;
        for (int route2_index = 0; route2_index < move[6]; route2_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]] = ImpDeliveryQuantityRoute2[route2_index];
            IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]] = ImpInventoryLevelRoute2[route2_index];
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
            IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + route1_index]] = ImpDeliveryQuantityRoute1[route1_index];
            IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[1]][move[2] + route1_index]] = ImpInventoryLevelRoute1[route1_index];

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
        // cout << "After modifying the routes" << endl;
        // IRPSolution.print_solution(IRPLR);

        boost_random_mechanism RandomRemoval;
        for (int insert_index = 0; insert_index < move[6]; insert_index++)
        {
            double QuantityToBeRemoved = ImpDeliveryQuantityRoute2[insert_index][move[0]];
            if (IntroduceNewQuantityOrNotRoute2[insert_index] == true)
            {
                LocalRebalanceTrigger = true;
                int WhileLoopCounter = 0;
                int vehicle_max_length = IRPSolution.Route[move[0]][move[1]].size();
                while (QuantityToBeRemoved > 0)
                {
                    // cout << "QuantityToBeRemoved: " << QuantityToBeRemoved << endl;
                    double selected_quantity = 0;
                    int selected_quantity_customer = IRPLR.Retailers.size() + 1;
                    vector<double> QuantityCandidates;
                    vector<int> CustomerCandidates;
                    for (int customer = 0; customer < memory.QuantityStructure.size(); customer++)
                    {
                        if (IRPSolution.VehicleAllocation[customer][move[0]] == move[1] && memory.QuantityStructure[customer][move[0]] > 0 && IRPSolution.DeliveryQuantity[customer][move[0]] > 0.00001)
                        {
                            bool AddCandidate = true;
                            // cout << "Customer inserted:";
                            for (int except_index = 0; except_index < move[6]; except_index++)
                            {
                                // cout << IRPSolution.Route[move[0]][move[1]][move[2] + except_index] << ",";
                                if (customer == IRPSolution.Route[move[0]][move[1]][move[2] + except_index])
                                {
                                    AddCandidate = false;
                                }
                            }
                            // cout << endl;
                            if (AddCandidate)
                            {
                                QuantityCandidates.push_back(memory.QuantityStructure[customer][move[0]]);
                                CustomerCandidates.push_back(customer);
                            }
                        }
                    }
                    // for (int test = 0; test < QuantityCandidates.size(); test++)
                    // {
                    //     cout << QuantityCandidates[test] << "," << CustomerCandidates[test] << endl;
                    // }
                    assert(QuantityCandidates.size() > 0);
                    int SelectedIndex = RandomRemoval.random_number_generator(0, QuantityCandidates.size() - 1, generator);
                    selected_quantity = QuantityCandidates[SelectedIndex];
                    selected_quantity_customer = CustomerCandidates[SelectedIndex];

                    // cout << "Selected:" << selected_quantity << "," << selected_quantity_customer << endl;
                    // cout << "Before IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]]:" << IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]];

                    IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] -= min(QuantityToBeRemoved, selected_quantity);
                    assert(IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] >= 0);
                    // cout << ", after IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]]:" << IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] << endl;
                    assert(IRPSolution.VehicleAllocation[selected_quantity_customer][move[0]] < IRPLR.NumberOfVehicles);
                    double TempChangeInTotalQuantity = 0.0; // Initialize appropriately
                    double TempStockOut = 0.0;              // Initialize appropriately
                    double TempVehicleOverload = 0.0;       // Initialize appropriately
                    if (move[0] == 0)
                    {
                        AdjustQuantityAndInventoryLevel(
                            IRPLR.Retailers[selected_quantity_customer].InventoryBegin,
                            move[0],
                            move[1],
                            IRPSolution.DeliveryQuantity[selected_quantity_customer],
                            IRPSolution.InventoryLevel[selected_quantity_customer],
                            IRPSolution.VehicleLoad,
                            IRPSolution.VehicleAllocation,
                            TempChangeInTotalQuantity,
                            TempStockOut,
                            TempVehicleOverload,
                            selected_quantity_customer,
                            IRPLR);
                    }
                    else
                    {
                        AdjustQuantityAndInventoryLevel(
                            IRPSolution.InventoryLevel[selected_quantity_customer][move[0] - 1],
                            move[0],
                            move[1],
                            IRPSolution.DeliveryQuantity[selected_quantity_customer],
                            IRPSolution.InventoryLevel[selected_quantity_customer],
                            IRPSolution.VehicleLoad,
                            IRPSolution.VehicleAllocation,
                            TempChangeInTotalQuantity,
                            TempStockOut,
                            TempVehicleOverload,
                            selected_quantity_customer,
                            IRPLR);
                    }

                    memory.UpdateQuantityStructure(IRPLR, IRPSolution, selected_quantity_customer, move[0], IRPSolution.VehicleAllocation[selected_quantity_customer][move[0]]);
                    QuantityToBeRemoved -= min(QuantityToBeRemoved, selected_quantity);

                    WhileLoopCounter++;
                    assert(WhileLoopCounter <= vehicle_max_length && "While loop for removing quantity is running too long");
                }
            }
        }

        // cout << "After quantity removal route1" << endl;
        // IRPSolution.print_solution(IRPLR);

        for (int insert_index = 0; insert_index < move[5]; insert_index++)
        {
            double QuantityToBeRemoved = ImpDeliveryQuantityRoute1[insert_index][move[0]];
            if (IntroduceNewQuantityOrNotRoute1[insert_index] == true)
            {
                LocalRebalanceTrigger = true;
                int WhileLoopCounter = 0;
                int vehicle_max_length = 10*IRPSolution.Route[move[0]][move[3]].size();
                while (QuantityToBeRemoved > 0)
                {
                    // cout << "QuantityToBeRemoved: " << QuantityToBeRemoved << endl;
                    double selected_quantity = 0;
                    int selected_quantity_customer = IRPLR.Retailers.size() + 1;
                    vector<double> QuantityCandidates;
                    vector<int> CustomerCandidates;
                    for (int customer = 0; customer < memory.QuantityStructure.size(); customer++)
                    {
                        if (IRPSolution.VehicleAllocation[customer][move[0]] == move[3] && memory.QuantityStructure[customer][move[0]] > 0&& IRPSolution.DeliveryQuantity[customer][move[0]] > 0.00001)
                        {
                            bool AddCandidate = true;
                            // cout << "Customer inserted:";
                            for (int except_index = 0; except_index < move[5]; except_index++)
                            {
                                // cout << IRPSolution.Route[move[0]][move[1]][move[2] + except_index] << ",";
                                if (customer == IRPSolution.Route[move[0]][move[3]][move[4] + except_index])
                                {
                                    AddCandidate = false;
                                }
                            }
                            // cout << endl;
                            if (AddCandidate)
                            {
                                QuantityCandidates.push_back(memory.QuantityStructure[customer][move[0]]);
                                CustomerCandidates.push_back(customer);
                            }
                        }
                    }
                    // for (int test = 0; test < QuantityCandidates.size(); test++)
                    // {
                    //     cout << QuantityCandidates[test] << "," << CustomerCandidates[test] << endl;
                    // }
                    assert(QuantityCandidates.size() > 0);
                    int SelectedIndex = RandomRemoval.random_number_generator(0, QuantityCandidates.size() - 1, generator);
                    selected_quantity = QuantityCandidates[SelectedIndex];
                    selected_quantity_customer = CustomerCandidates[SelectedIndex];

                    // cout << "Selected:" << selected_quantity << "," << selected_quantity_customer << endl;
                    // cout << "Before IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]]:" << IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]];

                    IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] -= min(QuantityToBeRemoved, selected_quantity);
                    assert(IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] >= 0);
                    // cout << ", after IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]]:" << IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]];
                    assert(IRPSolution.VehicleAllocation[selected_quantity_customer][move[0]] < IRPLR.NumberOfVehicles);
                    double TempChangeInTotalQuantity = 0.0; // Initialize appropriately
                    double TempStockOut = 0.0;              // Initialize appropriately
                    double TempVehicleOverload = 0.0;       // Initialize appropriately
                    if (move[0] == 0)
                    {
                        AdjustQuantityAndInventoryLevel(
                            IRPLR.Retailers[selected_quantity_customer].InventoryBegin,
                            move[0],
                            move[3],
                            IRPSolution.DeliveryQuantity[selected_quantity_customer],
                            IRPSolution.InventoryLevel[selected_quantity_customer],
                            IRPSolution.VehicleLoad,
                            IRPSolution.VehicleAllocation,
                            TempChangeInTotalQuantity,
                            TempStockOut,
                            TempVehicleOverload,
                            selected_quantity_customer,
                            IRPLR);
                    }
                    else
                    {

                        AdjustQuantityAndInventoryLevel(
                            IRPSolution.InventoryLevel[selected_quantity_customer][move[0] - 1],
                            move[0],
                            move[3],
                            IRPSolution.DeliveryQuantity[selected_quantity_customer],
                            IRPSolution.InventoryLevel[selected_quantity_customer],
                            IRPSolution.VehicleLoad,
                            IRPSolution.VehicleAllocation,
                            TempChangeInTotalQuantity,
                            TempStockOut,
                            TempVehicleOverload,
                            selected_quantity_customer,
                            IRPLR);
                    }

                    // cout << ", after update IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]]:" << IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] << endl;
                    memory.UpdateQuantityStructure(IRPLR, IRPSolution, selected_quantity_customer, move[0], IRPSolution.VehicleAllocation[selected_quantity_customer][move[0]]);
                    QuantityToBeRemoved -= min(QuantityToBeRemoved, selected_quantity);

                    WhileLoopCounter++;
                    assert(WhileLoopCounter <= vehicle_max_length && "While loop for removing quantity is running too long");
                }
            }
        }

        // cout<<"After quantity removal route2"<<endl;
        // IRPSolution.print_solution(IRPLR);
        // for (int customer = 0; customer < IRPSolution.DeliveryQuantity.size(); customer++)
        // {
        //     cout << "customer" << customer << ", " << IRPSolution.DeliveryQuantity[customer][move[0]] << ",IRPSolution.VehicleAllocation[customer][move[0]]" << IRPSolution.VehicleAllocation[customer][move[0]] << ", IRPSolution.VisitOrder[customer][move[0]]:" << IRPSolution.VisitOrder[customer][move[0]] << endl;
        // }
        bool cleaned_route1 = false;
        while (cleaned_route1 == false)
        {
            cleaned_route1 = true;
            for (int customer = 0; customer < IRPSolution.Route[move[0]][move[1]].size(); customer++)
            {
                if (IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][customer]][move[0]] < 0.00001)
                {

                    cleaned_route1 = false;
                    int customer_candidate = IRPSolution.Route[move[0]][move[1]][customer];
                    IRPSolution.UnallocatedCustomers[move[0]].push_back(customer_candidate);
                    IRPSolution.Route[move[0]][move[1]].erase(IRPSolution.Route[move[0]][move[1]].begin() + customer,
                                                              IRPSolution.Route[move[0]][move[1]].begin() + customer + 1);

                    for (int next_customer = customer; next_customer < IRPSolution.Route[move[0]][move[1]].size(); next_customer++)
                    {

                        IRPSolution.VisitOrder[IRPSolution.Route[move[0]][move[1]][next_customer]][move[0]] -= 1;
                    }
                    IRPSolution.VisitOrder[customer_candidate][move[0]] = IRPLR.Retailers.size() + 1;
                    IRPSolution.VehicleAllocation[customer_candidate][move[0]] = IRPLR.NumberOfVehicles + 1;
                    break;
                }
            }
        }

        bool cleaned_route2 = false;
        while (cleaned_route2 == false)
        {
            cleaned_route2 = true;
            for (int customer = 0; customer < IRPSolution.Route[move[0]][move[3]].size(); customer++)
            {
                if (IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[3]][customer]][move[0]] < 0.00001)
                {

                    cleaned_route2 = false;
                    int customer_candidate = IRPSolution.Route[move[0]][move[3]][customer];
                    IRPSolution.UnallocatedCustomers[move[0]].push_back(customer_candidate);
                    IRPSolution.Route[move[0]][move[3]].erase(IRPSolution.Route[move[0]][move[3]].begin() + customer,
                                                              IRPSolution.Route[move[0]][move[3]].begin() + customer + 1);

                    for (int next_customer = customer; next_customer < IRPSolution.Route[move[0]][move[3]].size(); next_customer++)
                    {
                        IRPSolution.VisitOrder[IRPSolution.Route[move[0]][move[3]][next_customer]][move[0]] -= 1;
                    }
                    IRPSolution.VisitOrder[customer_candidate][move[0]] = IRPLR.Retailers.size() + 1;
                    IRPSolution.VehicleAllocation[customer_candidate][move[0]] = IRPLR.NumberOfVehicles + 1;
                    break;
                }
            }
        }
        if(LocalRebalanceTrigger)
        {
            cout << "Local Rebalance Triggered" << endl;
        }
        // cout << "After all modification" << endl;
        // IRPSolution.print_solution(IRPLR);
        // cout << "Copy of memory" << endl;
        // vector<vector<double>> CopyQuantityStructure(memory.QuantityStructure);       // This is a local structure, keep it updated all time
        // vector<vector<double>> CopyTotalSlackPerVehicle(memory.TotalSlackPerVehicle); // This is a local structure, keep it updated all time
        // vector<double> CopyTotalSlackPerDay(memory.TotalSlackPerDay);                 // This is a local structure, keep it updated all time

        // for (int i = 0; i < CopyTotalSlackPerVehicle.size(); i++)
        // {
        //     for (int j = 0; j < CopyTotalSlackPerVehicle[i].size(); j++)
        //     {
        //         cout << "Total slack for vehicle " << j << " on day " << i << ": " << CopyTotalSlackPerVehicle[i][j] << endl;
        //     }
        // }

        // cout << "Status of QuantityStructure(Slack):" << endl;
        // for (int i = 0; i < CopyQuantityStructure.size(); i++)
        // {
        //     for (int j = 0; j < CopyQuantityStructure[i].size(); j++)
        //     {
        //         cout << CopyQuantityStructure[i][j] << ",";
        //     }
        //     cout << endl;
        // }
        // for (int i = 0; i < CopyTotalSlackPerDay.size(); i++)
        // {
        //     cout << "Total slack for day " << i << ": " << CopyTotalSlackPerDay[i] << endl;
        // }

        // memory.PopulateQuantityStructure(IRPLR, IRPSolution);

        // cout << "Actual of memory" << endl;

        // for (int i = 0; i < memory.TotalSlackPerVehicle.size(); i++)
        // {
        //     for (int j = 0; j < memory.TotalSlackPerVehicle[i].size(); j++)
        //     {
        //         cout << "Total slack for vehicle " << j << " on day " << i << ": " << memory.TotalSlackPerVehicle[i][j] << endl;
        //     }
        // }

        // cout << "Status of QuantityStructure(Slack):" << endl;
        // for (int i = 0; i < memory.QuantityStructure.size(); i++)
        // {
        //     for (int j = 0; j < memory.QuantityStructure[i].size(); j++)
        //     {
        //         cout << memory.QuantityStructure[i][j] << ",";
        //     }
        //     cout << endl;
        // }
        // for (int i = 0; i < memory.TotalSlackPerDay.size(); i++)
        // {
        //     cout << "Total slack for day " << i << ": " << memory.TotalSlackPerDay[i] << endl;
        // }
        // for (int i = 0; i < memory.QuantityStructure.size(); i++)
        // {
        //     for (int j = 0; j < memory.QuantityStructure[i].size(); j++)
        //     {
        //         assert(fabs(memory.QuantityStructure[i][j] - CopyQuantityStructure[i][j]) < 0.00001);
        //     }
        // }
        // for (int i = 0; i < memory.TotalSlackPerVehicle.size(); i++)
        // {
        //     assert(fabs(memory.TotalSlackPerDay[i] - CopyTotalSlackPerDay[i]) < 0.00001);
        //     for (int j = 0; j < memory.TotalSlackPerVehicle[i].size(); j++)
        //     {
        //         assert(fabs(memory.TotalSlackPerVehicle[i][j] - CopyTotalSlackPerVehicle[i][j]) < 0.00001);
        //     }
        // }

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