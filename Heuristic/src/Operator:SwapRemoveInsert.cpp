#include "lib.h"

int solution_improvement::OperatorSwapRemoveInsert(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, double &PenaltyMoreThanCapacity, preprocessing &memory, set<vector<int>> &SwapRemoveInsertPair,
                                                   int &min_remove_length, int &max_remove_length,
                                                   int &min_insert_length, int &max_insert_length)
{

    cout << "================================" << endl;
    cout << "OperatorSwapRemoveInsert" << endl;
    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);

    memory.PopulateQuantityStructure(IRPLR, IRPSolution);
    // IRPSolution.print_solution(IRPLR);
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, IRPSolution.ViolationMoreThanCapacity);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut: " << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << "\t ViolationMoreThanCapacity:" << IRPSolution.ViolationMoreThanCapacity << "\t PenaltyMoreThanCapacity:" << PenaltyMoreThanCapacity << "\t LR_objv:" << LR_objv << endl;

    // IRPSolution.print_solution(IRPLR);
    // for(int i=0; i<IRPSolution.TransportationCostPerRoute.size(); i++)
    // {
    //     for(int j=0; j<IRPSolution.TransportationCostPerRoute[i].size(); j++)
    //     {
    //         cout<<"TransportationCostPerRoute["<<i<<"]["<<j<<"]:"<<IRPSolution.TransportationCostPerRoute[i][j]<<",";
    //     }
    //     cout << endl;
    // }
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
    vector<bool> IntroduceNewQuantityOrNotInsert;
    double ImpTotalSlackPerVehicle;

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
    //  The quantity part of a move only depends on WHICH customers are removed/inserted, not on where they   //
    //  are placed. Therefore:                                                                                //
    //   (1) the removal part is evaluated once per (pick_allocated_position, remove_length) and reused for   //
    //       every pick_unallocated_position;                                                                 //
    //   (2) pure insertions (remove_length == 0) are evaluated once per                                      //
    //       (pick_unallocated_position, insert_length) and reused for every pick_allocated_position.         //
    //  Only the routing cost (ConcatenateSwapInsertRemove) is evaluated for every position. The arithmetic   //
    //  performed is exactly the one of evaluating each candidate from scratch, so the search is unchanged.  //
    //  All buffers are allocated once per operator call and reused, avoiding per-candidate heap traffic.     //
    //                                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct RemovalState
    {
        bool computed = false;
        vector<vector<double>> DeliveryQuantity; // [remove_index][day]
        vector<vector<double>> InventoryLevel;   // [remove_index][day]
        vector<vector<double>> VehicleLoad;      // [day][vehicle]
        double SlackDeduct = 0;
        double ChangeInTotalQuantity = 0;
        double StockOut = 0;
        double VehicleOverload = 0;
    };
    struct InsertionResult
    {
        bool computed = false;
        bool fail = false;
        vector<vector<double>> DeliveryQuantity; // [insert_index][day]
        vector<vector<double>> InventoryLevel;   // [insert_index][day]
        vector<char> IntroduceNewQuantity;       // [insert_index]
        vector<vector<double>> VehicleLoad;      // [day][vehicle]
        double ChangeInTotalQuantity = 0;
        double StockOut = 0;
        double VehicleOverload = 0;
        double TotalSlackPerVehicle = 0;
    };
    const int MaxRemoveRows = max(max_remove_length, 0);
    const int MaxInsertRows = max(max_insert_length, 0);
    vector<RemovalState> Removal(MaxRemoveRows + 1);
    for (int remove_length = 0; remove_length <= MaxRemoveRows; remove_length++)
    {
        Removal[remove_length].DeliveryQuantity.resize(remove_length);
        Removal[remove_length].InventoryLevel.resize(remove_length);
    }
    vector<InsertionResult> PureInsertionCache; // indexed by pick_unallocated_position * (MaxInsertRows + 1) + insert_length
    InsertionResult ScratchInsertion;
    vector<int> CandidateCustomers(MaxRemoveRows + MaxInsertRows);
    vector<const double *> CandidateRows(MaxRemoveRows + MaxInsertRows);

    // Evaluate the removal of Route[pick_day][pick_vehicle][pick_allocated_position, +remove_length)
    auto ComputeRemoval = [&](int pick_day, int pick_vehicle, int pick_allocated_position, int remove_length, RemovalState &rem)
    {
        rem.SlackDeduct = 0;
        for (int remove_index = 0; remove_index < remove_length; remove_index++)
        {
            const int customer = IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index];
            rem.DeliveryQuantity[remove_index] = IRPSolution.DeliveryQuantity[customer];
            rem.InventoryLevel[remove_index] = IRPSolution.InventoryLevel[customer];
            rem.SlackDeduct += memory.QuantityStructure[customer][pick_day];
        }
        rem.StockOut = IRPSolution.ViolationStockOut;
        rem.VehicleOverload = IRPSolution.ViolationMoreThanCapacity;
        rem.ChangeInTotalQuantity = 0;
        rem.VehicleLoad = IRPSolution.VehicleLoad;
        // Get change in delivery quantity and inventory level for the removed customers
        for (int remove_index = 0; remove_index < remove_length; remove_index++)
        {
            int customer = IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index];
            rem.VehicleLoad[pick_day][pick_vehicle] = rem.VehicleLoad[pick_day][pick_vehicle] - IRPSolution.DeliveryQuantity[customer][pick_day];
            rem.DeliveryQuantity[remove_index][pick_day] = 0;
            rem.ChangeInTotalQuantity = rem.ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[customer][pick_day];
            // The following code adjust the delivery quantity and inventory level of the removed customer across all affected periods.
            double &previous_inventory_level = (pick_day == 0) ? IRPLR.Retailers[customer].InventoryBegin : IRPSolution.InventoryLevel[customer][pick_day - 1];
            AdjustQuantityAndInventoryLevel(
                previous_inventory_level,
                pick_day,
                pick_vehicle,
                rem.DeliveryQuantity[remove_index],
                rem.InventoryLevel[remove_index],
                rem.VehicleLoad,
                IRPSolution.VehicleAllocation,
                rem.ChangeInTotalQuantity,
                rem.StockOut,
                rem.VehicleOverload,
                customer,
                IRPLR);
        }
        rem.computed = true;
    };

    // Evaluate inserting UnallocatedCustomers[pick_day][pick_unallocated_position, +insert_length) on top of a removal state
    auto EvaluateInsertion = [&](int pick_day, int pick_vehicle, int pick_allocated_position, int pick_unallocated_position,
                                 int remove_length, int insert_length, const RemovalState &rem, InsertionResult &out)
    {
        if ((int)out.DeliveryQuantity.size() < insert_length)
        {
            out.DeliveryQuantity.resize(insert_length);
            out.InventoryLevel.resize(insert_length);
            out.IntroduceNewQuantity.resize(insert_length);
        }
        out.fail = false; // if insert does not lead to an increase on delivery quantity, this move fails
        out.VehicleLoad = rem.VehicleLoad;
        out.ChangeInTotalQuantity = rem.ChangeInTotalQuantity;
        out.StockOut = rem.StockOut;
        out.VehicleOverload = rem.VehicleOverload;
        out.TotalSlackPerVehicle = memory.TotalSlackPerVehicle[pick_day][pick_vehicle];

        // Candidates for the supplier availability check: removed customers first, then inserted customers
        int NumberOfCandidates = 0;
        for (int remove_index = 0; remove_index < remove_length; ++remove_index)
        {
            CandidateCustomers[NumberOfCandidates] = IRPSolution.Route[pick_day][pick_vehicle][pick_allocated_position + remove_index];
            CandidateRows[NumberOfCandidates] = rem.DeliveryQuantity[remove_index].data();
            NumberOfCandidates++;
        }
        for (int insert_index = 0; insert_index < insert_length; ++insert_index)
        {
            const int customer = IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index];
            out.DeliveryQuantity[insert_index] = IRPSolution.DeliveryQuantity[customer];
            out.InventoryLevel[insert_index] = IRPSolution.InventoryLevel[customer];
            out.IntroduceNewQuantity[insert_index] = false;
            out.DeliveryQuantity[insert_index][pick_day] = 0.0;
            CandidateCustomers[NumberOfCandidates] = customer;
            CandidateRows[NumberOfCandidates] = out.DeliveryQuantity[insert_index].data();
            NumberOfCandidates++;
        }

        // Get change in delivery quantity and inventory level for the inserted customers
        for (int insert_index = 0; insert_index < insert_length; insert_index++)
        {
            int customer = IRPSolution.UnallocatedCustomers[pick_day][pick_unallocated_position + insert_index];
            // Supplier availability is evaluated with this customer's delivery on pick_day still at 0
            // (rows of later inserted customers are untouched, rows of earlier ones are final).
            const double SupplierAvailable = SupplierAvailabilityForCandidateDay(
                IRPLR, IRPSolution, pick_day, CandidateCustomers.data(), CandidateRows.data(), NumberOfCandidates);

            double &previous_inventory_level = (pick_day == 0) ? IRPLR.Retailers[customer].InventoryBegin : IRPSolution.InventoryLevel[customer][pick_day - 1];
            vector<double> &InsertDelivery = out.DeliveryQuantity[insert_index];
            InsertDelivery[pick_day] = DeliveryMax(
                IRPLR.Vehicle.capacity,
                out.VehicleLoad[pick_day][pick_vehicle],
                IRPLR.Retailers[customer].InventoryMax,
                IRPLR.Retailers[customer].Demand,
                previous_inventory_level);
            if (UseVehicleSlack && InsertDelivery[pick_day] < 0.00001) // vehicle slack (can be disabled with --UseVehicleSlack NO)
            {
                InsertDelivery[pick_day] = DeliveryMaxAllowingCapacityViolation(
                    IRPLR.Retailers[customer].InventoryMax,
                    IRPLR.Retailers[customer].Demand,
                    previous_inventory_level);
                InsertDelivery[pick_day] = min(InsertDelivery[pick_day], out.TotalSlackPerVehicle - rem.SlackDeduct);
                out.TotalSlackPerVehicle = out.TotalSlackPerVehicle - InsertDelivery[pick_day];
                out.IntroduceNewQuantity[insert_index] = true;
            }
            const double quantityBeforeSupplierLimit = InsertDelivery[pick_day];
            InsertDelivery[pick_day] = min(quantityBeforeSupplierLimit, max(0.0, SupplierAvailable));
            if (out.IntroduceNewQuantity[insert_index])
            {
                out.TotalSlackPerVehicle += quantityBeforeSupplierLimit - InsertDelivery[pick_day];
            }

            if (InsertDelivery[pick_day] < 0.001)
            {
                out.fail = true;
            }
            else
            {
                if (out.IntroduceNewQuantity[insert_index] == false)
                {
                    out.ChangeInTotalQuantity += InsertDelivery[pick_day] - IRPSolution.DeliveryQuantity[customer][pick_day];
                    out.VehicleLoad[pick_day][pick_vehicle] = out.VehicleLoad[pick_day][pick_vehicle] + InsertDelivery[pick_day];
                }
                assert(InsertDelivery[pick_day] != 0);
                double checkNewDeliveryQuantityCustomerInsert = InsertDelivery[pick_day];
                AdjustQuantityAndInventoryLevel(
                    previous_inventory_level,
                    pick_day,
                    pick_vehicle,
                    InsertDelivery,
                    out.InventoryLevel[insert_index],
                    out.VehicleLoad,
                    IRPSolution.VehicleAllocation,
                    out.ChangeInTotalQuantity,
                    out.StockOut,
                    out.VehicleOverload,
                    customer,
                    IRPLR);
                assert(fabs(checkNewDeliveryQuantityCustomerInsert - InsertDelivery[pick_day]) < 1e-6);
            }
        }

        if (out.fail == false)
        {
            if (SupplierAvailabilityForCandidateDay(
                    IRPLR, IRPSolution, pick_day, CandidateCustomers.data(), CandidateRows.data(), NumberOfCandidates) < -0.00001)
            {
                out.fail = true;
            }
        }
    };

    time(&accumulate_start_time);
    try
    {
        for (select_pair = SwapRemoveInsertPair.begin(); select_pair != SwapRemoveInsertPair.end(); select_pair++)
        {
            const vector<int> &pair = (*select_pair);
            int pick_day = pair[0];
            int pick_vehicle = pair[1];
            const int RouteSize = IRPSolution.Route[pick_day][pick_vehicle].size();
            const int UnallocatedSize = IRPSolution.UnallocatedCustomers[pick_day].size();

            const size_t PureInsertionCacheSize = (size_t)(UnallocatedSize + 1) * (MaxInsertRows + 1);
            if (PureInsertionCache.size() < PureInsertionCacheSize)
            {
                PureInsertionCache.resize(PureInsertionCacheSize);
            }
            for (size_t i = 0; i < PureInsertionCacheSize; i++)
            {
                PureInsertionCache[i].computed = false;
            }

            for (int pick_allocated_position = 0; pick_allocated_position <= RouteSize; pick_allocated_position++) // A position in this vehicle
            {
                for (int remove_length = 0; remove_length <= MaxRemoveRows; remove_length++)
                {
                    Removal[remove_length].computed = false;
                }
                for (int pick_unallocated_position = 0; pick_unallocated_position <= UnallocatedSize; pick_unallocated_position++) // A position in the unallocated customer
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
                                    if (pick_unallocated_position + insert_length <= UnallocatedSize && pick_allocated_position + remove_length <= RouteSize) // Make sure the operator does not go outside the range
                                    {
                                        time(&total_end_time);
                                        double total_ls_time = difftime(total_end_time, total_start_time);
                                        assert(total_ls_time - AccumulatedTimeHGS >= 0);
                                        if (total_ls_time - MainAlgorithmTimeLimit - AccumulatedTimeHGS > -0.01)
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
                                            RemovalState &rem = Removal[remove_length];
                                            if (!rem.computed)
                                            {
                                                ComputeRemoval(pick_day, pick_vehicle, pick_allocated_position, remove_length, rem);
                                            }
                                            InsertionResult *res = &ScratchInsertion;
                                            if (remove_length == 0)
                                            {
                                                // Pure insertion: quantities do not depend on pick_allocated_position
                                                res = &PureInsertionCache[(size_t)pick_unallocated_position * (MaxInsertRows + 1) + insert_length];
                                                if (!res->computed)
                                                {
                                                    EvaluateInsertion(pick_day, pick_vehicle, pick_allocated_position, pick_unallocated_position, remove_length, insert_length, rem, *res);
                                                    res->computed = true;
                                                }
                                            }
                                            else
                                            {
                                                EvaluateInsertion(pick_day, pick_vehicle, pick_allocated_position, pick_unallocated_position, remove_length, insert_length, rem, *res);
                                            }

                                            if (res->fail == false) // If insert fails, then we do not need to calculate the objective value, because it is infeasible
                                            {
                                                working_solutionCounter++;
                                                double NewRouteCost = memory.ConcatenateSwapInsertRemove(pick_day, pick_vehicle, pick_allocated_position, remove_length,
                                                                                                         pick_unallocated_position, insert_length,
                                                                                                         IRPSolution, IRPLR, memory);
                                                double NewTotalTransportationCost = IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle] + NewRouteCost;
                                                double NewTotalDelivery = IRPSolution.TotalDelivery + res->ChangeInTotalQuantity;
                                                double NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                                                double temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, res->StockOut, PenaltyMoreThanCapacity, res->VehicleOverload);

                                                if (LR_objv - temp_LR_objv > 0.00001)
                                                {
                                                    LR_objv = temp_LR_objv;
                                                    move[0] = pick_day;
                                                    move[1] = pick_vehicle;
                                                    move[2] = pick_allocated_position;
                                                    move[3] = pick_unallocated_position;
                                                    move[4] = remove_length;
                                                    move[5] = insert_length;
                                                    whether_improved_or_not = 1;
                                                    ImpVehicleLoad = res->VehicleLoad;
                                                    ImpDeliveryQuantityCustomerRemove.clear();
                                                    ImpInventoryLevelCustomerRemove.clear();
                                                    ImpDeliveryQuantityCustomerInsert.clear();
                                                    ImpInventoryLevelCustomerInsert.clear();
                                                    for (int remove_index = 0; remove_index < remove_length; remove_index++)
                                                    {
                                                        ImpDeliveryQuantityCustomerRemove.push_back(rem.DeliveryQuantity[remove_index]);
                                                        ImpInventoryLevelCustomerRemove.push_back(rem.InventoryLevel[remove_index]);
                                                    }
                                                    for (int insert_index = 0; insert_index < insert_length; insert_index++)
                                                    {
                                                        ImpDeliveryQuantityCustomerInsert.push_back(res->DeliveryQuantity[insert_index]);
                                                        ImpInventoryLevelCustomerInsert.push_back(res->InventoryLevel[insert_index]);
                                                    }
                                                    IntroduceNewQuantityOrNotInsert.assign(res->IntroduceNewQuantity.begin(), res->IntroduceNewQuantity.begin() + insert_length);
                                                    ImpStockOut = res->StockOut;
                                                    ImpLogisticRatio = NewLogisticRatio;
                                                    ImpTotalTransportationCost = NewTotalTransportationCost;
                                                    ImpTotalDelivery = NewTotalDelivery;
                                                    ImpVehicleOverload = res->VehicleOverload;
                                                    ImpTotalSlackPerVehicle = res->TotalSlackPerVehicle;
                                                }
                                            }
                                            else
                                            {
                                                not_working_solutionCounter++;
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
    cout << "whether_improved_or_not:" << whether_improved_or_not << ";";
    cout << "ImpLogisticRatio:" << ImpLogisticRatio << ", ImpStockOut:" << ImpStockOut << endl;
    cout << "Working solutions:" << working_solutionCounter << ";";
    cout << "Not working solutions:" << not_working_solutionCounter << endl;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //       Make up the solution, adjust its inventory level and route       //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    if (whether_improved_or_not == 1) // whether_improved_or_not==1
    {

        cout << "pick_day:" << move[0] << ", " << "pick_vehicle:" << move[1] << ", " << "pick_allocated_position:" << move[2] << ", " << "pick_unallocated_position:" << move[3] << ", " << "remove_length:" << move[4] << ", " << "insert_length:" << move[5] << endl;
        // cout << "Before updating affected visits" << endl;
        // for (int i = 0; i < memory.QuantityStructure.size(); i++)
        // {
        //     cout << "Retailer " << i << ";";
        //     for (int j = 0; j < memory.QuantityStructure[i].size(); j++)
        //     {
        //         cout << memory.QuantityStructure[i][j] << ",";
        //     }
        //     cout << endl;
        // }

        // cout<<ImpLogisticRatio<<"\t"<<ImpStockOut<<endl;
        bool LocalRebalanceTrigger = false;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;
        IRPSolution.ViolationMoreThanCapacity = ImpVehicleOverload;
        IRPSolution.TotalTransportationCost = ImpTotalTransportationCost;
        IRPSolution.TotalDelivery = ImpTotalDelivery;
        for (int insert_index = 0; insert_index < move[5]; insert_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index]] = ImpDeliveryQuantityCustomerInsert[insert_index];
            IRPSolution.InventoryLevel[IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index]] = ImpInventoryLevelCustomerInsert[insert_index];

            memory.UpdateQuantityStructure(IRPLR, IRPSolution, IRPSolution.UnallocatedCustomers[move[0]][move[3] + insert_index], move[0], move[1]);
        }
        for (int remove_index = 0; remove_index < move[4]; remove_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]] = ImpDeliveryQuantityCustomerRemove[remove_index];
            IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]] = ImpInventoryLevelCustomerRemove[remove_index];
            // for (int i = 0; i < IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]].size(); i++)
            // {
            //     cout << IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]][i] << ",";
            // }
            // cout << endl;
            // for (int i = 0; i < IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]].size(); i++)
            // {
            //     cout << IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]][i] << ",";
            // }
            // cout << endl;

            memory.UpdateQuantityStructure(IRPLR, IRPSolution, IRPSolution.Route[move[0]][move[1]][move[2] + remove_index], move[0], IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[1]][move[2] + remove_index]][move[0]]);
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

        // cout << "After updating affected visits" << endl;
        // for (int i = 0; i < memory.QuantityStructure.size(); i++)
        // {
        //     cout << "Retailer " << i << ";";
        //     for (int j = 0; j < memory.QuantityStructure[i].size(); j++)
        //     {
        //         cout << memory.QuantityStructure[i][j] << ",";
        //     }
        //     cout << endl;
        // }
        // cout << "Before quantity removal on existing visits" << endl;
        // IRPSolution.print_solution(IRPLR);
        boost_random_mechanism RandomRemoval;
        for (int insert_index = 0; insert_index < move[5]; insert_index++)
        {
            double QuantityToBeRemoved = ImpDeliveryQuantityCustomerInsert[insert_index][move[0]];
            // cout << "QuantityToBeRemoved: " << QuantityToBeRemoved << endl;
            if (IntroduceNewQuantityOrNotInsert[insert_index] == true)
            {
                LocalRebalanceTrigger = true;
                int WhileLoopCounter = 0;
                int vehicle_max_length = 10 * IRPSolution.Route[move[0]][move[1]].size();
                while (QuantityToBeRemoved > 0)
                {
                    double selected_quantity = 0;
                    int selected_quantity_customer = IRPLR.Retailers.size() + 1;
                    vector<double> QuantityCandidates;
                    vector<int> CustomerCandidates;
                    for (int customer = 0; customer < memory.QuantityStructure.size(); customer++)
                    {
                        if (IRPSolution.VehicleAllocation[customer][move[0]] == move[1] && memory.QuantityStructure[customer][move[0]] > 0 && IRPSolution.DeliveryQuantity[customer][move[0]] > 0)
                        {
                            bool AddCandidate = true;
                            // cout << "Customer inserted:";
                            for (int except_index = 0; except_index < move[5]; except_index++)
                            {
                                // cout << IRPSolution.Route[move[0]][move[1]][move[2] +  except_index] << ",";
                                if (customer == IRPSolution.Route[move[0]][move[1]][move[2] +  except_index])
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

                    IRPSolution.DeliveryQuantity[selected_quantity_customer][move[0]] -= min(QuantityToBeRemoved, selected_quantity);
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

        // for (int customer = 0; customer < IRPSolution.DeliveryQuantity.size(); customer++)
        // {
        //     cout << "customer" << customer << ", " << IRPSolution.DeliveryQuantity[customer][move[0]] << ",IRPSolution.VehicleAllocation[customer][move[0]]" << IRPSolution.VehicleAllocation[customer][move[0]] << ", IRPSolution.VisitOrder[customer][move[0]]:" << IRPSolution.VisitOrder[customer][move[0]] << endl;
        // }
        bool cleaned = false;
        while (cleaned == false)
        {
            cleaned = true;
            for (int customer = 0; customer < IRPSolution.Route[move[0]][move[1]].size(); customer++)
            {
                if (IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[1]][customer]][move[0]] < 0.00001)
                {

                    cleaned = false;
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

    cout << "================================" << endl;
    return whether_improved_or_not;
}