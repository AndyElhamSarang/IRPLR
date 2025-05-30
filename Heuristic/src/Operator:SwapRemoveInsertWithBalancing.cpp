#include "lib.h"
int solution_improvement::OperatorSwapRemoveInsertWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory,
                                                    int &min_remove_length, int &max_remove_length,
                                                    int &min_insert_length, int &max_insert_length)
{
    // This operator permits feasible solutions only

    // cout << "Swap with rebalance starting solution" << endl;
   

    int whether_improved_or_not = 0;
    int CountingInfeasibleCase = 0;
    double CurrentLogisticRatio = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    // IRPSolution.print_solution(IRPLR);
    // cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    CurrentLogisticRatio = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    // cout << "CurrentLogisticRatio :" << CurrentLogisticRatio << endl;

    vector<vector<vector<int>>> ImpRoute(IRPSolution.Route);
    vector<vector<int>> ImpUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
    vector<vector<double>> ImpVehicleLoad(IRPSolution.VehicleLoad);
    vector<vector<double>> ImpDeliveryQuantity(IRPSolution.DeliveryQuantity);
    vector<vector<double>> ImpInventoryLevel(IRPSolution.InventoryLevel);
    vector<vector<int>> ImpVehicleAllocation(IRPSolution.VehicleAllocation);
    vector<vector<int>> ImpVisitOrder(IRPSolution.VisitOrder);
    double ImpStockOut = 0;
    int solutionCounter = 0;
    for (int pick_day = 0; pick_day < IRPSolution.Route.size(); pick_day++) // For a day
    {
        for (int pick_vehicle = 0; pick_vehicle < IRPSolution.Route[pick_day].size(); pick_vehicle++) // For a vehicle
        {
            for (int pick_allocated_customer = 0; pick_allocated_customer <= IRPSolution.Route[pick_day][pick_vehicle].size(); pick_allocated_customer++) // A position in this vehicle
            {
                for (int pick_unallocated_customer = 0; pick_unallocated_customer <= IRPSolution.UnallocatedCustomers[pick_day].size(); pick_unallocated_customer++) // A position in the unallocated customer
                {
                    for (int remove_length = min_remove_length; remove_length <= max_remove_length; remove_length++)
                    {
                        for (int insert_length = min_insert_length; insert_length <= max_insert_length; insert_length++)
                        {
                            if (remove_length != 0 || insert_length != 0) // It is either a remove or insert
                            {
                                if (remove_length != 0 && insert_length == 0 && pick_unallocated_customer > 0)
                                {
                                    // Do nothing if it is just a remove do nothing for a poitision > 0 in the unallocated customer 
                                }
                                else
                                {
                                    if (pick_unallocated_customer + insert_length <= IRPSolution.UnallocatedCustomers[pick_day].size() && pick_allocated_customer + remove_length <= IRPSolution.Route[pick_day][pick_vehicle].size()) // Make sure the operator does not go outside the range
                                    {
                                    

                                        // cout << "solution: " << solutionCounter << endl;
                                        vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                                        vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                                        vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                                        vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                                        vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                                        vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);
                                        vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);

                                        TempRoute[pick_day][pick_vehicle].insert(TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_customer,
                                                                                 TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_customer,
                                                                                 TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_customer + insert_length);
                                        TempUnallocatedCustomers[pick_day].erase(TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_customer,
                                                                                 TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_customer + insert_length);
                                        TempUnallocatedCustomers[pick_day].insert(TempUnallocatedCustomers[pick_day].begin() + pick_unallocated_customer,
                                                                                  TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_customer + insert_length,
                                                                                  TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_customer + insert_length + remove_length);
                                        TempRoute[pick_day][pick_vehicle].erase(TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_customer + insert_length,
                                                                                TempRoute[pick_day][pick_vehicle].begin() + pick_allocated_customer + insert_length + remove_length);

                                        // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder);

                                        // Reset the visit order

                                        for (int a = 0; a < TempVisitOrder.size(); a++)
                                        {
                                            for (int b = 0; b < TempVisitOrder[a].size(); b++)
                                            {
                                                TempVehicleAllocation[a][b] = IRPLR.NumberOfVehicles + 1;
                                                TempVisitOrder[a][b] = IRPLR.Retailers.size() + 1;
                                            }
                                        }

                                        // Update the visit order

                                        for (int period = 0; period < TempRoute.size(); period++) // For this time period
                                        {
                                            for (int retailer = 0; retailer < IRPLR.Retailers.size(); retailer++) // Check each retailers
                                            {
                                                for (int vehicle = 0; vehicle < TempRoute[period].size(); vehicle++) // index pick_vehicle for vehicle
                                                {
                                                    for (int position = 0; position < TempRoute[period][vehicle].size(); position++) // index k for position
                                                    {
                                                        if (TempRoute[period][vehicle][position] == retailer)
                                                        {
                                                            TempVehicleAllocation[retailer][period] = vehicle;
                                                            TempVisitOrder[retailer][period] = position;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        int BalancingCounter = 0;
                                        int FeasibleRebalanceOrNot = 0;
                                        double NewLogisticRatio = 0;
                                        while (BalancingCounter < 1)
                                        {
                                            // cout << "Balancing attempt:" << BalancingCounter << endl;
                                            NewLogisticRatio = OperatorBalancing(IRPLR, memory, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder, CountingInfeasibleCase, FeasibleRebalanceOrNot);
                                            BalancingCounter++;
                                        }
                                        if (FeasibleRebalanceOrNot == 1)
                                        {
                                            // cout << "NewLogisticRatio: " << NewLogisticRatio << ", CurrentLogisticRatio: " << CurrentLogisticRatio << endl;
                                            if (NewLogisticRatio < CurrentLogisticRatio)
                                            {
                                                whether_improved_or_not = 1;
                                                ImpRoute = TempRoute;
                                                ImpUnallocatedCustomers = TempUnallocatedCustomers;
                                                ImpVehicleLoad = TempVehicleLoad;
                                                ImpDeliveryQuantity = TempDeliveryQuantity;
                                                ImpInventoryLevel = TempInventoryLevel;
                                                ImpVehicleAllocation = TempVehicleAllocation;
                                                ImpVisitOrder = TempVisitOrder;
                                            }
                                        }
                                        solutionCounter++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // cout << "Total # of solutions explored:" << solutionCounter << endl;
    // cout << "Total # of solutions which are infeasible by balancing:" << CountingInfeasibleCase << endl;
    // cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    if (whether_improved_or_not == 1)
    {
        IRPSolution.Route = ImpRoute;
        IRPSolution.UnallocatedCustomers = ImpUnallocatedCustomers;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.DeliveryQuantity = ImpDeliveryQuantity;
        IRPSolution.InventoryLevel = ImpInventoryLevel;
        IRPSolution.VehicleAllocation = ImpVehicleAllocation;
        IRPSolution.VisitOrder = ImpVisitOrder;
        IRPSolution.ViolationStockOut = ImpStockOut;

        IRPSolution.GetLogisticRatio(IRPLR);
        assert(fabs(IRPSolution.ViolationStockOut - ImpStockOut) < 0.001);
    }

    return whether_improved_or_not;
}