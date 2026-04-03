#include "lib.h"

int solution_improvement::OperatorSwapTwoRoutesOnSingleDay(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory, set<vector<int>> &SwapTwoRoutesOnSingleDayPair, set<vector<int>> SwapTwoRoutesOnSingleDayPairToReconsider,
                                                           int &min_Swap_length1, int &max_Swap_length1,
                                                           int &min_Swap_length2, int &max_Swap_length2)
{

    double accumulated_time = 0;
    time_t accumulate_start_time;
    time_t accumulate_end_time;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut: " << IRPSolution.ViolationStockOut << endl;

    // IRPSolution.print_solution(IRPLR);
    // for (int i = 0; i < IRPSolution.TransportationCostPerRoute.size(); i++)
    // {
    //     for (int j = 0; j < IRPSolution.TransportationCostPerRoute[i].size(); j++)
    //     {
    //         cout << "TransportationCostPerRoute[" << i << "][" << j << "]:" << IRPSolution.TransportationCostPerRoute[i][j] << ",";
    //     }
    //     cout << endl;
    // }
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut);
    double objv_begin = LR_objv;
    // cout << "IRPSolution.TotalTransportationCost:" << IRPSolution.TotalTransportationCost << ", IRPSolution.TotalDelivery:" << IRPSolution.TotalDelivery << ", LR objv:" << LR_objv << endl;
    vector<int> move;
    for (int i = 0; i < 8; i++)
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
    vector<vector<double>> ImpDeliveryQuantityRoute2;
    vector<vector<double>> ImpInventoryLevelRoute2;
    vector<vector<double>> ImpVehicleLoad;
    double ImpStockOut = 0;
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
                                    time(&LS_end_time);
                                    double total_ls_time = difftime(LS_end_time, LS_start_time);
                                    if (total_ls_time - LocalSearchTimeLimit > 0.00001)
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
                                        /////////////////////////////////////////////////////////////////////////////////
                                        // Initialize all data needed to capture the improving solution
                                        // cout<<"pick_day: "<<pick_day<<", pick_vehicle1:"<<pick_vehicle1<<", pick_position_in_vehicle1:"<<pick_position_in_vehicle1<<", pick_vehicle2:"<<pick_vehicle2<<", pick_position_in_vehicle2:"<<pick_position_in_vehicle2<<", Swap_length1:"<<Swap_length1<<", Swap_length2:"<<Swap_length2<<endl;
                                        vector<vector<double>> NewDeliveryQuantityRoute1;
                                        vector<vector<double>> NewInventoryLevelRoute1;
                                        for (int route1_index = pick_position_in_vehicle1; route1_index < pick_position_in_vehicle1 + Swap_length1; route1_index++)
                                        {
                                            NewDeliveryQuantityRoute1.push_back(IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle1][route1_index]]);
                                            NewInventoryLevelRoute1.push_back(IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle1][route1_index]]);
                                        }
                                        assert(NewDeliveryQuantityRoute1.size() == Swap_length1);
                                        assert(NewInventoryLevelRoute1.size() == Swap_length1);
                                        vector<vector<double>> NewDeliveryQuantityRoute2;
                                        vector<vector<double>> NewInventoryLevelRoute2;
                                        for (int route2_index = pick_position_in_vehicle2; route2_index < pick_position_in_vehicle2 + Swap_length2; route2_index++)
                                        {
                                            NewDeliveryQuantityRoute2.push_back(IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle2][route2_index]]);
                                            NewInventoryLevelRoute2.push_back(IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle2][route2_index]]);
                                        }
                                        assert(NewDeliveryQuantityRoute2.size() == Swap_length2);
                                        assert(NewInventoryLevelRoute2.size() == Swap_length2);

                                        double NewStockOut = IRPSolution.ViolationStockOut;
                                        double ChangeInTotalQuantity = 0;
                                        vector<vector<double>> CopyVehicleLoad = IRPSolution.VehicleLoad;

                                        // cout << "intialize all data needed to capture the improving solution" << endl;
                                        // cout << "SelectedVehicleLoad1: " << CopyVehicleLoad[pick_day][pick_vehicle1] << endl;
                                        // cout << "SelectedVehicleLoad2: " << CopyVehicleLoad[pick_day][pick_vehicle2] << endl;
                                        // cout << "ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                        // cout << "NewDeliveryQuantityRoute1" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute1" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewDeliveryQuantityRoute2" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute2" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        // Get change in delivery quantity and inventory level
                                        int Whether_insert_fail = 0; // if insert does not lead to an increase on delivery quantity, this move fails, therefore skip the evalution
                                        // Remove customers from route 1 and route 2
                                        // Make delivery quantity 0 for removed customers
                                        // Record the change in total delivery quantity
                                        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
                                        {
                                            CopyVehicleLoad[pick_day][pick_vehicle1] = CopyVehicleLoad[pick_day][pick_vehicle1] - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day];
                                            NewDeliveryQuantityRoute1[route1_index][pick_day] = 0;
                                            ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day];
                                        }
                                        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
                                        {
                                            CopyVehicleLoad[pick_day][pick_vehicle2] = CopyVehicleLoad[pick_day][pick_vehicle2] - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day];
                                            NewDeliveryQuantityRoute2[route2_index][pick_day] = 0;
                                            ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day];
                                        }

                                        // cout << "Remove customers from route 1 and route 2" << endl;
                                        // cout << " Make delivery quantity 0 for removed customers" << endl;
                                        // cout << "Record the change in total delivery quantity" << endl;
                                        // cout << "CopyVehicleLoad[pick_day][pick_vehicle1]: " << CopyVehicleLoad[pick_day][pick_vehicle1] << endl;
                                        // cout << "CopyVehicleLoad[pick_day][pick_vehicle2]: " << CopyVehicleLoad[pick_day][pick_vehicle2] << endl;
                                        // cout << "ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                        // cout << "NewDeliveryQuantityRoute1" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute1" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewDeliveryQuantityRoute2" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute2" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // Update delivery quantity and inventory level for inserted customers from route 2 to route 1 and inserted customers from route 1 to route 2
                                        for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
                                        {
                                            // Add delivery quantity for inserted customers from route 2 to route 1
                                            if (pick_day == 0)
                                            {
                                                NewDeliveryQuantityRoute1[route1_index][pick_day] = DeliveryMax(
                                                    IRPLR.Vehicle.capacity,
                                                    CopyVehicleLoad[pick_day][pick_vehicle2],
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].InventoryMax,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].Demand,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].InventoryBegin);
                                            }
                                            else
                                            {

                                                NewDeliveryQuantityRoute1[route1_index][pick_day] = DeliveryMax(
                                                    IRPLR.Vehicle.capacity,
                                                    CopyVehicleLoad[pick_day][pick_vehicle2],
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].InventoryMax,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].Demand,
                                                    IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day - 1]);
                                            }

                                            if (NewDeliveryQuantityRoute1[route1_index][pick_day] < 0.001)
                                            {
                                                Whether_insert_fail = 1;
                                            }
                                            else
                                            {
                                                ChangeInTotalQuantity += NewDeliveryQuantityRoute1[route1_index][pick_day];
                                                CopyVehicleLoad[pick_day][pick_vehicle2] = CopyVehicleLoad[pick_day][pick_vehicle2] + NewDeliveryQuantityRoute1[route1_index][pick_day];

                                                // cout << "Add new delivery quantity" << endl;
                                                // cout << "CopyVehicleLoad[pick_day][pick_vehicle1]: " << CopyVehicleLoad[pick_day][pick_vehicle1] << endl;
                                                // cout << "CopyVehicleLoad[pick_day][pick_vehicle2]: " << CopyVehicleLoad[pick_day][pick_vehicle2] << endl;
                                                // cout << "ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                                // cout << "NewDeliveryQuantityRoute1" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityRoute1.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityRoute1[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityRoute1[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelRoute1" << endl;
                                                // for (int i = 0; i < NewInventoryLevelRoute1.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelRoute1[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelRoute1[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewDeliveryQuantityRoute2" << endl;
                                                // for (int i = 0; i < NewDeliveryQuantityRoute2.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewDeliveryQuantityRoute2[i].size(); j++)
                                                //     {
                                                //         cout << NewDeliveryQuantityRoute2[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;
                                                // cout << "NewInventoryLevelRoute2" << endl;
                                                // for (int i = 0; i < NewInventoryLevelRoute2.size(); i++)
                                                // {
                                                //     for (int j = 0; j < NewInventoryLevelRoute2[i].size(); j++)
                                                //     {
                                                //         cout << NewInventoryLevelRoute2[i][j] << ",";
                                                //     }
                                                //     cout << endl;
                                                // }
                                                // cout << endl;

                                                assert(NewDeliveryQuantityRoute1[route1_index][pick_day] != 0);

                                                if (pick_day == 0)
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]].InventoryBegin,
                                                        pick_day,
                                                        pick_vehicle2,
                                                        NewDeliveryQuantityRoute1[route1_index],
                                                        NewInventoryLevelRoute1[route1_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index],
                                                        IRPLR);
                                                }
                                                else
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index]][pick_day - 1],
                                                        pick_day,
                                                        pick_vehicle2,
                                                        NewDeliveryQuantityRoute1[route1_index],
                                                        NewInventoryLevelRoute1[route1_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle1][pick_position_in_vehicle1 + route1_index],
                                                        IRPLR);
                                                }
                                            }
                                        }
                                        for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
                                        {
                                            if (pick_day == 0)
                                            {

                                                NewDeliveryQuantityRoute2[route2_index][pick_day] = DeliveryMax(
                                                    IRPLR.Vehicle.capacity,
                                                    CopyVehicleLoad[pick_day][pick_vehicle1],
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].InventoryMax,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].Demand,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].InventoryBegin);
                                            }
                                            else
                                            {

                                                NewDeliveryQuantityRoute2[route2_index][pick_day] = DeliveryMax(
                                                    IRPLR.Vehicle.capacity,
                                                    CopyVehicleLoad[pick_day][pick_vehicle1],
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].InventoryMax,
                                                    IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].Demand,
                                                    IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day - 1]);
                                            }

                                            if (NewDeliveryQuantityRoute2[route2_index][pick_day] < 0.001)
                                            {
                                                Whether_insert_fail = 1;
                                            }
                                            else
                                            {
                                                ChangeInTotalQuantity += NewDeliveryQuantityRoute2[route2_index][pick_day];
                                                CopyVehicleLoad[pick_day][pick_vehicle1] = CopyVehicleLoad[pick_day][pick_vehicle1] + NewDeliveryQuantityRoute2[route2_index][pick_day];

                                                assert(NewDeliveryQuantityRoute2[route2_index][pick_day] != 0);

                                                if (pick_day == 0)
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPLR.Retailers[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]].InventoryBegin,
                                                        pick_day,
                                                        pick_vehicle1,
                                                        NewDeliveryQuantityRoute2[route2_index],
                                                        NewInventoryLevelRoute2[route2_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index],
                                                        IRPLR);
                                                }
                                                else
                                                {
                                                    AdjustQuantityAndInventoryLevel(
                                                        IRPSolution.InventoryLevel[IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index]][pick_day - 1],
                                                        pick_day,
                                                        pick_vehicle1,
                                                        NewDeliveryQuantityRoute2[route2_index],
                                                        NewInventoryLevelRoute2[route2_index],
                                                        CopyVehicleLoad,
                                                        IRPSolution.VehicleAllocation,
                                                        ChangeInTotalQuantity,
                                                        NewStockOut,
                                                        IRPSolution.Route[pick_day][pick_vehicle2][pick_position_in_vehicle2 + route2_index],
                                                        IRPLR);
                                                }
                                            }
                                        }

                                        // cout << "Update delivery quantity and inventory level for inserted customers from route 2 to route 1 and inserted customers from route 1 to route 2" << endl;
                                        // cout << "CopyVehicleLoad[pick_day][pick_vehicle1]: " << CopyVehicleLoad[pick_day][pick_vehicle1] << endl;
                                        // cout << "CopyVehicleLoad[pick_day][pick_vehicle2]: " << CopyVehicleLoad[pick_day][pick_vehicle2] << endl;
                                        // cout << "ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                        // cout << "NewDeliveryQuantityRoute1" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute1" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute1.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute1[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute1[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewDeliveryQuantityRoute2" << endl;
                                        // for (int i = 0; i < NewDeliveryQuantityRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewDeliveryQuantityRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewDeliveryQuantityRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // cout << "NewInventoryLevelRoute2" << endl;
                                        // for (int i = 0; i < NewInventoryLevelRoute2.size(); i++)
                                        // {
                                        //     for (int j = 0; j < NewInventoryLevelRoute2[i].size(); j++)
                                        //     {
                                        //         cout << NewInventoryLevelRoute2[i][j] << ",";
                                        //     }
                                        //     cout << endl;
                                        // }
                                        // cout << endl;
                                        // assert(pick_day ==0);
                                        if (Whether_insert_fail == 0)
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
                                            // cout << "NewRoute1Cost: " << NewRoute1Cost << ", NewRoute2Cost: " << NewRoute2Cost << endl;
                                            double NewTotalTransportationCost =
                                                IRPSolution.TotalTransportationCost - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle1] - IRPSolution.TransportationCostPerRoute[pick_day][pick_vehicle2] + NewRoute1Cost + NewRoute2Cost;

                                            double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;
                                            double NewLogisticRatio = NewTotalTransportationCost / NewTotalDelivery;
                                            double temp_LR_objv = Calculate_la_relax_objv(NewLogisticRatio, PenaltyForStockOut, NewStockOut);
                                            //  cout<<"objv_begin:"<<objv_begin<<" temp_LR_objv:" << temp_LR_objv << endl;
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
                                                    whether_improved_or_not = 1; // whether_improved_or_not = 1;

                                                    ImpDeliveryQuantityRoute1.clear();
                                                    ImpInventoryLevelRoute1.clear();
                                                    ImpDeliveryQuantityRoute2.clear();
                                                    ImpInventoryLevelRoute2.clear();

                                                    for (int route1_index = 0; route1_index < Swap_length1; route1_index++)
                                                    {
                                                        ImpDeliveryQuantityRoute1.push_back(NewDeliveryQuantityRoute1[route1_index]);
                                                        ImpInventoryLevelRoute1.push_back(NewInventoryLevelRoute1[route1_index]);
                                                    }

                                                    for (int route2_index = 0; route2_index < Swap_length2; route2_index++)
                                                    {
                                                        ImpDeliveryQuantityRoute2.push_back(NewDeliveryQuantityRoute2[route2_index]);
                                                        ImpInventoryLevelRoute2.push_back(NewInventoryLevelRoute2[route2_index]);
                                                    }
                                                    ImpVehicleLoad = CopyVehicleLoad;
                                                    ImpStockOut = NewStockOut;
                                                    ImpLogisticRatio = NewLogisticRatio;
                                                    ImpTotalTransportationCost = NewTotalTransportationCost;
                                                    ImpTotalDelivery = NewTotalDelivery;

                                                    // cout << "Improving solution found" << endl;
                                                    // cout << "Update delivery quantity and inventory level for inserted customers from route 2 to route 1 and inserted customers from route 1 to route 2" << endl;
                                                    // cout << "CopyVehicleLoad[pick_day][pick_vehicle1]: " << CopyVehicleLoad[pick_day][pick_vehicle1] << endl;
                                                    // cout << "CopyVehicleLoad[pick_day][pick_vehicle2]: " << CopyVehicleLoad[pick_day][pick_vehicle2] << endl;
                                                    // cout << "ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
                                                    // cout << "NewDeliveryQuantityRoute1" << endl;
                                                    // for (int i = 0; i < NewDeliveryQuantityRoute1.size(); i++)
                                                    // {
                                                    //     for (int j = 0; j < NewDeliveryQuantityRoute1[i].size(); j++)
                                                    //     {
                                                    //         cout << NewDeliveryQuantityRoute1[i][j] << ",";
                                                    //     }
                                                    //     cout << endl;
                                                    // }
                                                    // cout << endl;
                                                    // cout << "NewInventoryLevelRoute1" << endl;
                                                    // for (int i = 0; i < NewInventoryLevelRoute1.size(); i++)
                                                    // {
                                                    //     for (int j = 0; j < NewInventoryLevelRoute1[i].size(); j++)
                                                    //     {
                                                    //         cout << NewInventoryLevelRoute1[i][j] << ",";
                                                    //     }
                                                    //     cout << endl;
                                                    // }
                                                    // cout << endl;
                                                    // cout << "NewDeliveryQuantityRoute2" << endl;
                                                    // for (int i = 0; i < NewDeliveryQuantityRoute2.size(); i++)
                                                    // {
                                                    //     for (int j = 0; j < NewDeliveryQuantityRoute2[i].size(); j++)
                                                    //     {
                                                    //         cout << NewDeliveryQuantityRoute2[i][j] << ",";
                                                    //     }
                                                    //     cout << endl;
                                                    // }
                                                    // cout << endl;
                                                    // cout << "NewInventoryLevelRoute2" << endl;
                                                    // for (int i = 0; i < NewInventoryLevelRoute2.size(); i++)
                                                    // {
                                                    //     for (int j = 0; j < NewInventoryLevelRoute2[i].size(); j++)
                                                    //     {
                                                    //         cout << NewInventoryLevelRoute2[i][j] << ",";
                                                    //     }
                                                    //     cout << endl;
                                                    // }
                                                    // cout << endl;
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
                                        { // If failed in evalute, this pair is kept for next iteration, treat it easy`
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
        // cout << "Improved solution found by SwapTwoRoutesSingleDay operator:" << endl;
        // cout << ImpLogisticRatio << "\t" << ImpStockOut << endl;
        // for(int i=0;i<move.size();i++)
        // {
        //     cout<<"move["<<i<<"]:"<<move[i]<<"\t";
        // }
        // cout<<endl;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.LogisticRatio = ImpLogisticRatio;
        IRPSolution.ViolationStockOut = ImpStockOut;
        for (int route2_index = 0; route2_index < move[6]; route2_index++)
        {
            IRPSolution.DeliveryQuantity[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]] = ImpDeliveryQuantityRoute2[route2_index];
            IRPSolution.InventoryLevel[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]] = ImpInventoryLevelRoute2[route2_index];
            IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[3]][move[4] + route2_index]][move[0]] = move[1];

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
            IRPSolution.VehicleAllocation[IRPSolution.Route[move[0]][move[1]][move[2] + route1_index]][move[0]] = move[3];

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
    return whether_improved_or_not;
}