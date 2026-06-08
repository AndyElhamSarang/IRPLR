#include "lib.h"
void solution_improvement::IteratedLocalSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory, solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s, solution &IRPSolution60s, file &read_file, int &MS_ITERATION)
{
    cout << "//////////////////////////////////////" << endl;
    cout << "//   Start Iterated Local Search    //" << endl;
    cout << "//////////////////////////////////////" << endl;
    // memory.PopulateLocalPrefixAndSuffix(IRPLR, IRPSolution);

    time_t rebalance_start_time;
    time_t rebalance_end_time;
    double TimeForHGS = 0;
    double total_rebalance_time = 0;
    IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
    
    time(&start_time);
    int DisturbanceCounter = 0;
    int AdjustPenalty= 0;
    double ScalarLagrangianRelaxation = InitialLagrangianScalar;
    int ToAdjustPenalty = ToTriggerAdjustment;
    int NumberOfInfeasibleSolution = 0;
    int NumberOfFeasibleSolution = 0;
    int FeasibleSolutionCounter = 0;
    int CountFeasibleLocalSearchOut = 0;
    int CountInFeasibleLocalSearchOut = 0;
    int NumberOfRebalance = 0;
    int NumberOfFeasibleRebalance = 0;
    int NumberOfRebalanceImproved = 0;
    double AccumulatedPrecentageRebalanceImprovement = 0;
    double RebalanceMaxPrecentageImprovement = 0;
    double RebalanceMinPrecentageImprovement = 0;
    bool RunHGSAtEnd = false;
    bool run_disturb_rebalance = false;
    // assert(UseSwapRemoveInsert!=UseSwapRemoveInsertRebalance);
    int LocalSearchCounter = 0;
    int MaxDisturbance = 3 * (IRPLR.NumberOfRetailers + 10 * IRPLR.TimeHorizon + 10 * IRPLR.NumberOfVehicles);
    try
    {
        while (DisturbanceCounter < MaxDisturbance)
        {
            time(&total_end_time);
            double total_ls_time = difftime(total_end_time, total_start_time);
            cout<< "Total time for local search so far:" << total_ls_time << "s, AccumulatedTimeHGS:" << AccumulatedTimeHGS << "s, MainAlgorithmTimeLimit:" << MainAlgorithmTimeLimit << "s." << endl;
            assert(total_ls_time - AccumulatedTimeHGS >= 0);
            if (total_ls_time - MainAlgorithmTimeLimit - AccumulatedTimeHGS > -0.01)
            {
                int time_limit_reached = total_ls_time;
                throw time_limit_reached;
            }
            // assert(UseSwapRemoveInsert!=UseSwapRemoveInsertRebalance);

            time_t LocalSearch_start_time;
            time_t LocalSearch_end_time;
            ///////////////////////////////////////////////////////////////////
            //                                                               //
            //		     Local Search to improve current solution            //
            //                                                               //
            ///////////////////////////////////////////////////////////////////
            time(&LocalSearch_start_time);
            cout << "---------------------------------------------" << endl;
            cout << "Start Local Search, iteration " << LocalSearchCounter << ", with Disturbance Counter: " << DisturbanceCounter << endl;
            cout << "---------------------------------------------" << endl;

            ImprovedLocalSearch(IRPLR, IRPSolution, ScalarLagrangianRelaxation, memory, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, DisturbanceCounter, RunHGSAtEnd);
            // LocalSearch(IRPLR, IRPSolution, ScalarLagrangianRelaxation, memory, GlobalBest, FirstImprovementSolution, IRPSolution30s,IRPSolution60s);
            time(&LocalSearch_end_time);
            double total_LocalSearch_time = difftime(LocalSearch_end_time, LocalSearch_start_time);

            cout << "---------------------------------------------" << endl;
            cout << "End Local Search" << endl;
            // IRPSolution.Validation(IRPLR);
            cout << "Total LocalSearch time:" << total_LocalSearch_time << "s." << endl;
            IRPSolution.GetLogisticRatio(IRPLR);
            // IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
            // IRPSolution.print_solution(IRPLR);
            cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
            cout << "ViolationStockOut" << IRPSolution.ViolationStockOut << "\t ScalarLagrangianRelaxation:" << ScalarLagrangianRelaxation << endl;

            cout << "---------------------------------------------" << endl;

            // if (IRPSolution.ViolationStockOut != 0)
            // {
            //     int whether_improved_insert = OperatorInsert(IRPLR, IRPSolution, ScalarLagrangianRelaxation, memory);
            // }
            // violation = OperatorRemove(IRPLR, IRPSolution,  ScalarLagrangianRelaxation, memory);

            if (IRPSolution.ViolationStockOut - 0 > 0.00001)
            {
                CountInFeasibleLocalSearchOut++;
            }
            else
            {
                CountFeasibleLocalSearchOut++;
                NumberOfFeasibleSolution++;
                FeasibleSolutionCounter++;
                cout << "!Feasible solution obtained" << endl;

                ///////////////////////////////////////////////////////////////////
                //                                                               //
                //		     Remove visit with 0 delivery quantity               //
                //                                                               //
                ///////////////////////////////////////////////////////////////////
                for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++) // Index i for retailer
                {
                    for (int j = 0; j < IRPSolution.DeliveryQuantity[i].size(); j++) // Index j for time period
                    {
                        if (IRPSolution.DeliveryQuantity[i][j] == 0 && IRPSolution.VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
                        {
                            IRPSolution.UnallocatedCustomers[j].push_back(IRPSolution.Route[j][IRPSolution.VehicleAllocation[i][j]][IRPSolution.VisitOrder[i][j]]);
                            IRPSolution.Route[j][IRPSolution.VehicleAllocation[i][j]].erase(IRPSolution.Route[j][IRPSolution.VehicleAllocation[i][j]].begin() + IRPSolution.VisitOrder[i][j], IRPSolution.Route[j][IRPSolution.VehicleAllocation[i][j]].begin() + IRPSolution.VisitOrder[i][j] + 1);

                            // Here insert code to achieve fast evaluation on travel time

                            for (int k = 0; k < IRPSolution.VisitOrder.size(); k++)
                            {
                                if (IRPSolution.VehicleAllocation[k][j] == IRPSolution.VehicleAllocation[i][j]) // Find visits in the same vehicle as Retailer i on day j
                                {
                                    if (IRPSolution.VisitOrder[k][j] > IRPSolution.VisitOrder[i][j]) // If this visit is after the visit of Retailer i
                                    {
                                        IRPSolution.VisitOrder[k][j] = IRPSolution.VisitOrder[k][j] - 1;
                                    }
                                }
                            }
                            IRPSolution.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
                            IRPSolution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
                        }
                        else
                        {
                            if (IRPSolution.VehicleAllocation[i][j] == IRPLR.NumberOfVehicles + 1)
                            {
                                assert(IRPSolution.DeliveryQuantity[i][j] == 0);
                            }
                        }
                    }
                }
                IRPSolution.GetLogisticRatio(IRPLR);
                cout << "Solution after removing customer with 0 delivery " << endl;
                cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;

                ///////////////////////////////////////////////////////////////////////////////

                if (GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
                {

                    GlobalBest = IRPSolution;
                    GlobalBest.LogisticRatio = IRPSolution.LogisticRatio;
                    run_disturb_rebalance = true;
                    time(&end_time_to_best);
                    GlobalBest.solution_time = difftime(end_time_to_best, start_time_to_best);
                    cout << "$GlobalBest solution is updated at time:" << GlobalBest.solution_time << " s,\t with " << "TotalTransportationCost:" << GlobalBest.TotalTransportationCost << ",\t TotalDelivery:" << GlobalBest.TotalDelivery << ",\t LogisticRatio:" << GlobalBest.LogisticRatio << ",\t at local search iteration:" << LocalSearchCounter << endl;
                    DisturbanceCounter = 0; // Reset disturbance counter when a better solution is found
                    RunHGSAtEnd = true;
                }
                
                bool Activate_rebalance = false;
                if (Activate_rebalance == true)
                {
                    cout << "!-----------------------------------" << endl;
                    cout << "!Run rebalance" << endl;
                }
                else if (Activate_rebalance == false)
                {
                    cout << "!Does not run rebalance after each local search" << endl;
                }
                if (Activate_rebalance == true)
                {

                    int is_Rebalace_infeasible = 0;
                    int counting_infeasible_case = 0;

                    // double LogisticRatioBeforeRebalance = GlobalBest.LogisticRatio;
                    // cout << "!LogisticRatio before rebalance:" << GlobalBest.LogisticRatio << endl;
                    // GlobalBest.print_solution(IRPLR);
                    // vector<vector<vector<int>>> TempRoute(GlobalBest.Route);
                    // vector<vector<int>> TempUnallocatedCustomers(GlobalBest.UnallocatedCustomers);
                    // vector<vector<double>> TempVehicleLoad(GlobalBest.VehicleLoad);
                    // vector<vector<double>> TempDeliveryQuantity(GlobalBest.DeliveryQuantity);
                    // vector<vector<double>> TempInventoryLevel(GlobalBest.InventoryLevel);
                    // vector<vector<int>> TempVehicleAllocation(GlobalBest.VehicleAllocation);
                    // vector<vector<int>> TempVisitOrder(GlobalBest.VisitOrder);

                    double LogisticRatioBeforeRebalance = IRPSolution.LogisticRatio;

                    // IRPSolution.print_solution(IRPLR);
                    vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                    vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                    vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                    vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                    vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                    vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);
                    vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);

                    time(&rebalance_start_time);
                    double LogisctiRatioAfterRebalance = numeric_limits<double>::max();
                    LogisctiRatioAfterRebalance = OperatorBalancing(IRPLR, memory, TempRoute, TempUnallocatedCustomers,
                                                                    TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel,
                                                                    TempVehicleAllocation, TempVisitOrder,
                                                                    counting_infeasible_case, is_Rebalace_infeasible);
                    time(&rebalance_end_time);
                    total_rebalance_time += difftime(rebalance_end_time, rebalance_start_time);
                    NumberOfRebalance++;
                    if (counting_infeasible_case == 0)
                    {
                        NumberOfFeasibleRebalance++;
                        cout << "!LogisticRatio before rebalance:" << LogisticRatioBeforeRebalance << ", LogisticRatio after feasible rebalance:" << LogisctiRatioAfterRebalance << "," << NumberOfRebalance << "," << total_rebalance_time << endl;
                        if (LogisticRatioBeforeRebalance - LogisctiRatioAfterRebalance > 0.00001)
                        {

                            IRPSolution.Route = TempRoute;
                            IRPSolution.UnallocatedCustomers = TempUnallocatedCustomers;
                            IRPSolution.VehicleLoad = TempVehicleLoad;
                            IRPSolution.DeliveryQuantity = TempDeliveryQuantity;
                            IRPSolution.InventoryLevel = TempInventoryLevel;
                            IRPSolution.VehicleAllocation = TempVehicleAllocation;
                            IRPSolution.VisitOrder = TempVisitOrder;
                            IRPSolution.GetLogisticRatio(IRPLR);
                            // IRPSolution.Validation(IRPLR);
                            // IRPSolution.print_solution(IRPLR);
                            cout << "After rebalance" << endl;
                            // IRPSolution.Validation(IRPLR);
                            if (LogisticRatioBeforeRebalance - LogisctiRatioAfterRebalance > 0.00001)
                            {
                                double RebalanceImprovement = ((LogisticRatioBeforeRebalance - LogisctiRatioAfterRebalance) / LogisticRatioBeforeRebalance) * 100;
                                AccumulatedPrecentageRebalanceImprovement += RebalanceImprovement;
                                if (RebalanceMaxPrecentageImprovement < RebalanceImprovement)
                                {
                                    RebalanceMaxPrecentageImprovement = RebalanceImprovement;
                                }
                                if (RebalanceMinPrecentageImprovement < 0.00001)
                                {
                                    RebalanceMinPrecentageImprovement = RebalanceImprovement;
                                }
                                else if (RebalanceMinPrecentageImprovement > RebalanceImprovement)
                                {
                                    RebalanceMinPrecentageImprovement = RebalanceImprovement;
                                }
                                NumberOfRebalanceImproved++;
                                cout << "!Rebalance obtained better feasible solution with objv:" << IRPSolution.LogisticRatio << "," << LogisctiRatioAfterRebalance << endl;
                            }
                            assert(fabs(IRPSolution.LogisticRatio - LogisctiRatioAfterRebalance) < 0.00001);
                            if (GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
                            {
                                GlobalBest = IRPSolution;
                                GlobalBest.LogisticRatio = IRPSolution.LogisticRatio;
                                run_disturb_rebalance = true;
                                time(&end_time_to_best);
                                GlobalBest.solution_time = difftime(end_time_to_best, start_time_to_best);
                                cout << "$GlobalBest solution is updated at time:" << GlobalBest.solution_time << " s,\t with " << "TotalTransportationCost:" << GlobalBest.TotalTransportationCost << ",\t TotalDelivery:" << GlobalBest.TotalDelivery << ",\t LogisticRatio:" << GlobalBest.LogisticRatio << ",\t at local search iteration:" << DisturbanceCounter << endl;
                                RunHGSAtEnd = true;
                                DisturbanceCounter = 0; // Reset disturbance counter when a better solution is found
                            }
                            
                        }
                    }

                    cout << "!-----------------------------------" << endl;
                }
            }

            /////////////////////////////////////////////////////
            //                                                 //
            //      Update Scalar for relaxation stockout      //
            //                                                 //
            /////////////////////////////////////////////////////

            if (min(DisturbanceCounter, AdjustPenalty) >= ToAdjustPenalty)
            {
                AdjustPenalty = 0;
                ScalarLagrangianRelaxation = ScalarLagrangianRelaxation * ToAdjustLagrangianScalar;
            }

            cout << "DisturbanceCounter:" << DisturbanceCounter << ", AdjustPenalty:" << AdjustPenalty << ", ToAdjustPenalty:" << ToAdjustPenalty << " ,ScalarLagrangianRelaxation:" << ScalarLagrangianRelaxation << endl;
            cout << "-----------------------------------------------" << endl;
            cout << "Start Disturbance Operator" << endl;
            if (run_disturb_rebalance == true)
            {
                run_disturb_rebalance = false;
                vector<vector<vector<int>>> DisturbRoute(GlobalBest.Route);
                vector<vector<int>> DisturbUnallocatedCustomers(GlobalBest.UnallocatedCustomers);
                vector<vector<double>> DisturbVehicleLoad(GlobalBest.VehicleLoad);
                vector<vector<double>> DisturbDeliveryQuantity(GlobalBest.DeliveryQuantity);
                vector<vector<double>> DisturbInventoryLevel(GlobalBest.InventoryLevel);
                vector<vector<int>> DisturbVehicleAllocation(GlobalBest.VehicleAllocation);
                vector<vector<int>> DisturbVisitOrder(GlobalBest.VisitOrder);
                int Disturb_counting_infeasible_case = 0;
                int Disturb_is_Rebalace_infeasible = 0;
                time(&rebalance_start_time);
                double Disturb_LogisctiRatioAfterRebalance = numeric_limits<double>::max();
                if (TypeOfRebalance == "GUARANTEE_FEASIBILITY")
                {
                    Disturb_LogisctiRatioAfterRebalance = OperatorBalancing_guarantee_feasibility(IRPLR, memory, DisturbRoute, DisturbUnallocatedCustomers,
                                                                                                  DisturbVehicleLoad, DisturbDeliveryQuantity, DisturbInventoryLevel,
                                                                                                  DisturbVehicleAllocation, DisturbVisitOrder,
                                                                                                  Disturb_counting_infeasible_case, Disturb_is_Rebalace_infeasible);
                }
                else if (TypeOfRebalance == "NOT_GUARANTEE_FEASIBILITY")
                {
                    Disturb_LogisctiRatioAfterRebalance = OperatorBalancing(IRPLR, memory, DisturbRoute, DisturbUnallocatedCustomers,
                                                                            DisturbVehicleLoad, DisturbDeliveryQuantity, DisturbInventoryLevel,
                                                                            DisturbVehicleAllocation, DisturbVisitOrder,
                                                                            Disturb_counting_infeasible_case, Disturb_is_Rebalace_infeasible);
                }

                cout << "After rebalance" << endl;


                time(&rebalance_end_time);
                total_rebalance_time += difftime(rebalance_end_time, rebalance_start_time);
                NumberOfRebalance++;
                if (Disturb_counting_infeasible_case == 0)
                {
                    NumberOfFeasibleRebalance++;
                    cout << "!LogisticRatio before disturb rebalance:" << GlobalBest.LogisticRatio << ", LogisticRatio after feasible rebalance:" << Disturb_LogisctiRatioAfterRebalance << "," << NumberOfRebalance << "," << total_rebalance_time << endl;
                    if (GlobalBest.LogisticRatio - Disturb_LogisctiRatioAfterRebalance > 0.00001)
                    {
                        double RebalanceImprovement = ((GlobalBest.LogisticRatio - Disturb_LogisctiRatioAfterRebalance) / GlobalBest.LogisticRatio) * 100;
                        AccumulatedPrecentageRebalanceImprovement += RebalanceImprovement;
                        if (RebalanceMaxPrecentageImprovement < RebalanceImprovement)
                        {
                            RebalanceMaxPrecentageImprovement = RebalanceImprovement;
                        }
                        if (RebalanceMinPrecentageImprovement < 0.00001)
                        {
                            RebalanceMinPrecentageImprovement = RebalanceImprovement;
                        }
                        else if (RebalanceMinPrecentageImprovement > RebalanceImprovement)
                        {
                            RebalanceMinPrecentageImprovement = RebalanceImprovement;
                        }
                        NumberOfRebalanceImproved++;
                        cout << "!Rebalance obtained better feasible solution with objv:" << GlobalBest.LogisticRatio << "," << Disturb_LogisctiRatioAfterRebalance << endl;

                        GlobalBest.Route = DisturbRoute;
                        GlobalBest.UnallocatedCustomers = DisturbUnallocatedCustomers;
                        GlobalBest.VehicleLoad = DisturbVehicleLoad;
                        GlobalBest.DeliveryQuantity = DisturbDeliveryQuantity;
                        GlobalBest.InventoryLevel = DisturbInventoryLevel;
                        GlobalBest.VehicleAllocation = DisturbVehicleAllocation;
                        GlobalBest.VisitOrder = DisturbVisitOrder;
                        GlobalBest.GetLogisticRatio(IRPLR);
                        // IRPSolution.print_solution(IRPLR);
                        cout << "After rebalance" << endl;
                        // IRPSolution.Validation(IRPLR);
                        // GlobalBest.Validation(IRPLR);

                        assert(fabs(GlobalBest.LogisticRatio - Disturb_LogisctiRatioAfterRebalance) < 0.00001);
                    }
                }
            }

            OperatorDisturb(IRPLR, GlobalBest, IRPSolution, DisturbanceCounter, MaxDisturbance);
            IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);
            IRPSolution.print_solution(IRPLR);
            cout << "End Disturbance Operator" << endl;
            cout << "------------------------------------------------" << endl;
            Global_total_iteration++;
            AdjustPenalty++;
            DisturbanceCounter++;
            // assert(DisturbanceCounter<1);
        }
    }
    catch (int time_limit_reached)
    {
        cout << "!Stop by time limit" << endl;
    }
    if (OutputSolutionJSON == "YES")
    {
        GlobalBest.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_final_solution_" + to_string(MS_ITERATION) + ".json");
    }
    if (OutputResults == 1)
    {

        Table << NumberOfRebalance << "," << NumberOfFeasibleRebalance << "," << NumberOfRebalanceImproved << "," << AccumulatedPrecentageRebalanceImprovement / NumberOfRebalance << "," << RebalanceMaxPrecentageImprovement << "," << RebalanceMinPrecentageImprovement << ",";
        Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << ",";
    }
    ///////////////////////////////////////////////
    //                                           //
    //                Routing                    //
    //                                           //
    ///////////////////////////////////////////////
    time(&HGS_start_time);
    if (ActivateHGS == "YES")
    {
        assert(ActivateHGS == "YES");
        if (RunHGSAtEnd == true)
        {
            cout << "Best solution before HGS" << endl;
            GlobalBest.print_solution(IRPLR);

            for (int i = 0; i < GlobalBest.Route.size(); i++)
            {
                int NumberOfCustomerOfDay = 0;
                for (int j = 0; j < GlobalBest.Route[i].size(); j++)
                {
                    NumberOfCustomerOfDay += GlobalBest.Route[i][j].size();
                }
                if (NumberOfCustomerOfDay > 1)
                {
                    GlobalBest.OutputCVRP(IRPLR, i, GlobalBest.Route[i]);
                    Routing.CallHGS(IRPLR);
                    GlobalBest.ReadCVRP_Solution(IRPLR, i, GlobalBest.Route[i]);
                }
            }
            // cout << "IRPLR.NumberOfRetailers:" << IRPLR.NumberOfRetailers << ", " << "IRPLR.Retailers.size():" << IRPLR.Retailers.size() << endl;
            for (int i = 0; i < GlobalBest.VisitOrder.size(); i++)
            {
                for (int j = 0; j < GlobalBest.VisitOrder[i].size(); j++)
                {
                    GlobalBest.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
                    GlobalBest.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
                }
            }

            GlobalBest.UnallocatedCustomers.clear();
            for (int i = 0; i < GlobalBest.Route.size(); i++) // For this time period
            {
                vector<int> TempUnallocatedCustomer;             // Look for unallcated customers at this time period
                for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
                {
                    int UnallocatedYesOrNo = 0;
                    for (int j = 0; j < GlobalBest.Route[i].size(); j++) // index j for vehicle
                    {
                        for (int k = 0; k < GlobalBest.Route[i][j].size(); k++) // index k for position
                        {

                            if (GlobalBest.Route[i][j][k] == x)
                            {
                                UnallocatedYesOrNo = 1;
                                GlobalBest.VehicleAllocation[x][i] = j;
                                GlobalBest.VisitOrder[x][i] = k;
                            }
                        }
                    }
                    if (UnallocatedYesOrNo == 0) // This customer is not visited
                    {
                        TempUnallocatedCustomer.push_back(x);
                    }
                }
                GlobalBest.UnallocatedCustomers.push_back(TempUnallocatedCustomer);
            }

            time(&end_time_to_best);
            GlobalBest.solution_time = difftime(end_time_to_best, start_time_to_best);
            cout << "$GlobalBest solution is updated at time:" << GlobalBest.solution_time << " s,\t with " << "TotalTransportationCost:" << GlobalBest.TotalTransportationCost << ",\t TotalDelivery:" << GlobalBest.TotalDelivery << ",\t LogisticRatio:" << GlobalBest.LogisticRatio << ",\t at local search iteration:" << DisturbanceCounter << endl;
        }
        else
        {
            cout << "Do not run HGS at the end." << endl;
        }
        if (OutputSolutionJSON == "YES")
        {
            GlobalBest.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_final_afterHGS_solution_" + to_string(MS_ITERATION) + ".json");
        }
        // cout << "Solution after Optimizing the routes on the best found" << endl;
        // GlobalBest.print_solution(IRPLR);
        // cout << "TotalTransportationCost:" << GlobalBest.TotalTransportationCost << "\t TotalDelivery:" << GlobalBest.TotalDelivery << "\t LogistcRatio:" << GlobalBest.LogisticRatio << endl;
    }
    time(&HGS_end_time);
    TimeForHGS = difftime(HGS_end_time, HGS_start_time);
    AccumulatedTimeHGS += TimeForHGS;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    time(&end_time);
    double total_time = difftime(end_time, start_time);
   
    if (OutputResults == 1)
    {

         Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << TimeForHGS << "," << total_time << ",";
    }
    // LNS_Destory(IRPLR, IRPSolution,Routing);
    // LNS_Repair(IRPLR, IRPSolution,Routing);
}
