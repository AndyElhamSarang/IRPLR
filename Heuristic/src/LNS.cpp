#include "lib.h"
void solution_improvement::LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory)
{
    // memory.PopulateLocalPrefixAndSuffix(IRPLR, IRPSolution);

    vector<vector<double>> OriginalInventoryLevel;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> tempInventoryLevel;
        int tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand;
            tempInventoryLevel.push_back(tempInventory);
        }
        OriginalInventoryLevel.push_back(tempInventoryLevel);
    }
    cout << "Inventory Level without any delivery:" << endl;
    cout << "          \tBegin\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << '\t';
    }
    cout << endl;
    for (int i = 0; i < OriginalInventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << ":\t" << IRPLR.Retailers[i].InventoryBegin << '\t';
        for (int j = 0; j < OriginalInventoryLevel[i].size() - 1; j++)
        {
            cout << OriginalInventoryLevel[i][j] << '\t';
        }
        cout << OriginalInventoryLevel[i][OriginalInventoryLevel[i].size() - 1] << endl;
    }
    cout << endl;

    IRPSolution.print_solution(IRPLR);
    vector<vector<double>> PossibleDeliveryQuantityDeduction;
    vector<int> NumberOfVisits;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        NumberOfVisits.push_back(0);
    }

    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                for (int x = 0; x < IRPLR.Retailers.size(); x++)
                {
                    if (IRPSolution.Route[i][j][k] == x)
                    {
                        NumberOfVisits[x]++;
                    }
                }
            }
        }
    }

    // cout << "NumberOfVisits" << endl;
    // for (int i = 0; i < NumberOfVisits.size(); i++)
    // {
    //     cout << NumberOfVisits[i] << ",";
    // }
    // cout << endl;

    time_t start_time;
    time_t end_time;
    time(&start_time);
    solution BestIRP_Solution(IRPSolution);
    BestIRP_Solution.GetLogisticRatio(IRPLR);
    cout << "Best logistic ratio:" << BestIRP_Solution.LogisticRatio << endl;
    int OperatorSwapCounter = 0;
    double PenaltyForStockOut = 1;
    int ToAdjustPenalty = 10;
    int ItForCurrentPenalty = 0;
    int NumberOfInfeasibleSolution = 0;
    int NumberOfFeasibleSolution = 0;
    int FeasibleSolutionCounter = 0;
    int BetterFeasibleSolutionCounter = 0;
    int UseSwapRemoveInsert = 0;
    int UseSwapRemoveInsertRebalance = 1;
    int UsePureSwap = 0;
    int NumberOfRebalance = 0;
    int NumberOfRebalanceImproved = 0;
    double AccumulatedPrecentageRebalanceImprovement = 0;
    // assert(UseSwapRemoveInsert!=UseSwapRemoveInsertRebalance);
    while (OperatorSwapCounter < 50)
    {
        // assert(UseSwapRemoveInsert!=UseSwapRemoveInsertRebalance);
        int min_remove_length = 0;
        int max_remove_length = 1;
        int min_insert_length = 0;
        int max_insert_length = 1;
        int whether_improved_swap = 0;
        int whether_improved_swap_rebalance = 0;
        if (UseSwapRemoveInsert == 1)
        {
            whether_improved_swap = OperatorSwapRemoveInsert(IRPLR, IRPSolution, Routing, PenaltyForStockOut, memory, min_remove_length, max_remove_length, min_insert_length, max_insert_length);
        }
        else if (UseSwapRemoveInsertRebalance == 1)
        {
            whether_improved_swap_rebalance = OperatorSwapRemoveInsertWithBalancing(IRPLR, IRPSolution, Routing, PenaltyForStockOut, memory, min_remove_length, max_remove_length, min_insert_length, max_insert_length);
        }
        else if (UsePureSwap == 1)
        {
            whether_improved_swap_rebalance = OperatorPureSwap(IRPLR, IRPSolution, Routing, PenaltyForStockOut, memory);
        }

        IRPSolution.GetLogisticRatio(IRPLR);
        cout << "Solution after iteration " << OperatorSwapCounter << endl;
        cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
        cout << "ViolationStockOut" << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << endl;
        // IRPSolution.print_solution(IRPLR);

        // if (IRPSolution.ViolationStockOut != 0)
        // {
        //     int whether_improved_insert = OperatorInsert(IRPLR, IRPSolution, PenaltyForStockOut, memory);
        // }
        // violation = OperatorRemove(IRPLR, IRPSolution,  PenaltyForStockOut, memory);

        ItForCurrentPenalty++;
        if (IRPSolution.ViolationStockOut - 0 > 0.001)
        {
            NumberOfInfeasibleSolution++;
            if (NumberOfInfeasibleSolution >= ToAdjustPenalty)
            {
                PenaltyForStockOut = PenaltyForStockOut * 2;
                ItForCurrentPenalty = 0;
                NumberOfInfeasibleSolution = 0;
            }
        }
        else
        {
            NumberOfFeasibleSolution++;
            FeasibleSolutionCounter++;
            cout << "!Feasible solution obtained" << endl;
            int is_Rebalace_infeasible = 0;
            int counting_infeasible_case = 0;
            bool Activate_rebalance = true;
            if (Activate_rebalance == true)
            {
                cout << "!Run rebalance" << endl;
            }
            else if (Activate_rebalance == false)
            {
                cout << "!Does not run rebalance" << endl;
            }
            if (Activate_rebalance == true)
            {
                double LogisticRatioBeforeRebalance = IRPSolution.LogisticRatio;
                cout << "!LogisticRatio before rebalance:" << IRPSolution.LogisticRatio << endl;

                vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);
                vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);

                double LogisctiRatioAfterRebalance = OperatorBalancing(IRPLR, memory, TempRoute, TempUnallocatedCustomers,
                                                                       TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel,
                                                                       TempVehicleAllocation, TempVisitOrder,
                                                                       counting_infeasible_case, is_Rebalace_infeasible);
                NumberOfRebalance++;
                if (counting_infeasible_case == 0)
                {
                    IRPSolution.Route = TempRoute;
                    IRPSolution.UnallocatedCustomers = TempUnallocatedCustomers;
                    IRPSolution.VehicleLoad = TempVehicleLoad;
                    IRPSolution.DeliveryQuantity = TempDeliveryQuantity;
                    IRPSolution.InventoryLevel = TempInventoryLevel;
                    IRPSolution.VehicleAllocation = TempVehicleAllocation;
                    IRPSolution.VisitOrder = TempVisitOrder;
                    IRPSolution.GetLogisticRatio(IRPLR);
                    if (LogisticRatioBeforeRebalance - LogisctiRatioAfterRebalance > 0.001)
                    {
                        AccumulatedPrecentageRebalanceImprovement += ((LogisticRatioBeforeRebalance - LogisctiRatioAfterRebalance) / LogisticRatioBeforeRebalance) * 100;
                        NumberOfRebalanceImproved++;
                        cout << "!Rebalance obtained better feasible solution with objv:" << IRPSolution.LogisticRatio << "," << LogisctiRatioAfterRebalance << endl;
                    }
                    assert(fabs(IRPSolution.LogisticRatio - LogisctiRatioAfterRebalance) < 0.001);
                }
            }
            if (BestIRP_Solution.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
            {
                BetterFeasibleSolutionCounter++;
                cout << "!Best solution is updated" << endl;
                BestIRP_Solution = IRPSolution;
                BestIRP_Solution.LogisticRatio = IRPSolution.LogisticRatio;
            }
            if (NumberOfFeasibleSolution >= ToAdjustPenalty)
            {
                PenaltyForStockOut = PenaltyForStockOut / 2;
                ItForCurrentPenalty = 0;
                NumberOfFeasibleSolution = 0;
            }
        }
        OperatorSwapCounter++;
    }

    ///////////////////////////////////////////////
    //                                           //
    //                Routing                    //
    //                                           //
    ///////////////////////////////////////////////

    // cout << "Best solution before HGS" << endl;
    // BestIRP_Solution.print_solution(IRPLR);

    for (int i = 0; i < BestIRP_Solution.Route.size(); i++)
    {
        int NumberOfCustomerOfDay = 0;
        for (int j = 0; j < BestIRP_Solution.Route[i].size(); j++)
        {
            NumberOfCustomerOfDay += BestIRP_Solution.Route[i][j].size();
        }
        if (NumberOfCustomerOfDay > 1)
        {
            BestIRP_Solution.OutputCVRP(IRPLR, i, BestIRP_Solution.Route[i]);
            Routing.CallHGS(IRPLR);
            BestIRP_Solution.ReadCVRP_Solution(IRPLR, i, BestIRP_Solution.Route[i]);
        }
    }
    // cout << "IRPLR.NumberOfRetailers:" << IRPLR.NumberOfRetailers << ", " << "IRPLR.Retailers.size():" << IRPLR.Retailers.size() << endl;
    for (int i = 0; i < BestIRP_Solution.VisitOrder.size(); i++)
    {
        for (int j = 0; j < BestIRP_Solution.VisitOrder[i].size(); j++)
        {
            BestIRP_Solution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
            BestIRP_Solution.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
        }
    }

    BestIRP_Solution.UnallocatedCustomers.clear();
    for (int i = 0; i < BestIRP_Solution.Route.size(); i++) // For this time period
    {
        vector<int> TempUnallocatedCustomer;             // Look for unallcated customers at this time period
        for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
        {
            int UnallocatedYesOrNo = 0;
            for (int j = 0; j < BestIRP_Solution.Route[i].size(); j++) // index j for vehicle
            {
                for (int k = 0; k < BestIRP_Solution.Route[i][j].size(); k++) // index k for position
                {

                    if (BestIRP_Solution.Route[i][j][k] == x)
                    {
                        UnallocatedYesOrNo = 1;
                        BestIRP_Solution.VehicleAllocation[x][i] = j;
                        BestIRP_Solution.VisitOrder[x][i] = k;
                    }
                }
            }
            if (UnallocatedYesOrNo == 0) // This customer is not visited
            {
                TempUnallocatedCustomer.push_back(x);
            }
        }
        BestIRP_Solution.UnallocatedCustomers.push_back(TempUnallocatedCustomer);
    }
    BestIRP_Solution.GetLogisticRatio(IRPLR);

    // cout << "Solution after Optimizing the routes on the best found" << endl;
    // BestIRP_Solution.print_solution(IRPLR);
    // cout << "TotalTransportationCost:" << BestIRP_Solution.TotalTransportationCost << "\t TotalDelivery:" << BestIRP_Solution.TotalDelivery << "\t LogistcRatio:" << BestIRP_Solution.LogisticRatio << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    time(&end_time);
    double total_time = difftime(end_time, start_time);
    double check_LogisticRatio = BestIRP_Solution.LogisticRatio;
    cout << "Best known results" << endl;
    BestIRP_Solution.print_solution(IRPLR);
    BestIRP_Solution.GetLogisticRatio(IRPLR);
    cout << check_LogisticRatio << "," << BestIRP_Solution.LogisticRatio << endl;
    assert(fabs(check_LogisticRatio - BestIRP_Solution.LogisticRatio) < 0.001);
    cout << "!NumberOfRebalance: " << NumberOfRebalance << ", NumberOfRebalanceImproved: " << NumberOfRebalanceImproved << endl;
    cout << "!FeasibleSolutionCounter: " << FeasibleSolutionCounter << endl;
    cout << "!BetterFeasibleSolutionCounter: " << BetterFeasibleSolutionCounter << endl;
    cout << "!Best Logistic Ratio: " << BestIRP_Solution.LogisticRatio << endl;
    cout << "!Time: " << total_time << endl;
    if (OutputResults == 1)
    {

        Table << NumberOfRebalance << "," << NumberOfRebalanceImproved << "," << AccumulatedPrecentageRebalanceImprovement / NumberOfRebalance << ",";
        Table << BestIRP_Solution.TotalTransportationCost << "," << BestIRP_Solution.TotalDelivery << "," << BestIRP_Solution.LogisticRatio << "," << total_time << ",";
    }
    // LNS_Destory(IRPLR, IRPSolution,Routing);
    // LNS_Repair(IRPLR, IRPSolution,Routing);
}
