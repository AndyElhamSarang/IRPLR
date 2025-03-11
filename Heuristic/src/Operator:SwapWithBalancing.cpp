#include "lib.h"
int solution_improvement::OperatorSwapWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory)
{
    // This operator permits feasible solutions only

    cout << "Swap starting solution" << endl;
    int min_sequence_length_1 = 0;
    int max_sequence_length_1 = 1;
    int min_sequence_length_2 = 0;
    int max_sequence_length_2 = 1;

    int whether_improved_or_not = 0;
    int CountingInfeasibleCase = 0;
    double CurrentLogisticRatio = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    CurrentLogisticRatio = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    cout << "CurrentLogisticRatio :" << CurrentLogisticRatio << endl;

    vector<vector<vector<int>>> ImpRoute(IRPSolution.Route);
    vector<vector<int>> ImpUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
    vector<vector<double>> ImpVehicleLoad(IRPSolution.VehicleLoad);
    vector<vector<double>> ImpDeliveryQuantity(IRPSolution.DeliveryQuantity);
    vector<vector<double>> ImpInventoryLevel(IRPSolution.InventoryLevel);
    vector<vector<int>> ImpVehicleAllocation(IRPSolution.VehicleAllocation);
    vector<vector<int>> ImpVisitOrder(IRPSolution.VisitOrder);
    double ImpStockOut = 0;
    int solutionCounter = 0;
    for (int i = 0; i < IRPSolution.Route.size(); i++) // For a day
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++) // For a vehicle
        {
            for (int k = 0; k <= IRPSolution.Route[i][j].size(); k++) // A position in this vehicle
            {
                for (int x = 0; x <= IRPSolution.UnallocatedCustomers[i].size(); x++) // A position in the unallocated customer
                {
                    for (int sequence_length_1 = min_sequence_length_1; sequence_length_1 <= max_sequence_length_1; sequence_length_1++)
                    {
                        for (int sequence_length_2 = min_sequence_length_2; sequence_length_2 <= max_sequence_length_2; sequence_length_2++)
                        {
                            if (sequence_length_1 != 0 || sequence_length_2 != 0)
                            {
                                if (x + sequence_length_2 <= IRPSolution.UnallocatedCustomers[i].size() && k + sequence_length_1 <= IRPSolution.Route[i][j].size())
                                {
                                    cout << "solution: " << solutionCounter << endl;
                                    vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                                    vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                                    vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                                    vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                                    vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                                    vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);
                                    vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);

                                    TempRoute[i][j].insert(TempRoute[i][j].begin() + k,
                                                           TempUnallocatedCustomers[i].begin() + x,
                                                           TempUnallocatedCustomers[i].begin() + x + sequence_length_2);
                                    TempUnallocatedCustomers[i].erase(TempUnallocatedCustomers[i].begin() + x,
                                                                      TempUnallocatedCustomers[i].begin() + x + sequence_length_2);
                                    TempUnallocatedCustomers[i].insert(TempUnallocatedCustomers[i].begin() + x,
                                                                       TempRoute[i][j].begin() + k + sequence_length_2,
                                                                       TempRoute[i][j].begin() + k + sequence_length_2 + sequence_length_1);
                                    TempRoute[i][j].erase(TempRoute[i][j].begin() + k + sequence_length_2,
                                                          TempRoute[i][j].begin() + k + sequence_length_2 + sequence_length_1);
                                    cout << "After altertion" << endl;
                                    for (int print_vehicle = 0; TempRoute[i].size(); print_vehicle++)
                                    {
                                        for (int print_customer = 0; print_customer < TempRoute[i].size(); print_customer++)
                                        {
                                            cout << TempRoute[i][print_vehicle][print_customer] << ",";
                                        }
                                        cout << endl;
                                    }
                                    cout << "Unallocated customer:" << endl;
                                    for (int print_unallocate_customer = 0; print_unallocate_customer < TempUnallocatedCustomers[i].size(); print_unallocate_customer++)
                                    {
                                        cout << TempUnallocatedCustomers[i][print_unallocate_customer] << ",";
                                    }
                                    cout << endl;

                                    // Update the visit order
                                    for (int a = 0; a < TempVisitOrder.size(); a++)
                                    {
                                        for (int b = 0; b < TempVisitOrder[a].size(); b++)
                                        {
                                            TempVehicleAllocation[a][b] = IRPLR.NumberOfVehicles + 1;
                                            TempVisitOrder[a][b] = IRPLR.Retailers.size() + 1;
                                        }
                                    }

                                    for (int period = 0; period < TempRoute.size(); period++) // For this time period
                                    {
                                        for (int retailer = 0; retailer < IRPLR.Retailers.size(); retailer++) // Check each retailers
                                        {
                                            for (int vehicle = 0; vehicle < TempRoute[period].size(); vehicle++) // index j for vehicle
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

                                    //PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder);
                                    int BalancingCounter = 0;
                                    int FeasibleRebalanceOrNot = 0;
                                    double NewLogisticRatio = 0;
                                    while (BalancingCounter < 1)
                                    {
                                        cout << "Balancing attempt:" << BalancingCounter << endl;
                                        NewLogisticRatio = OperatorBalancing(IRPLR, memory, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder, CountingInfeasibleCase, FeasibleRebalanceOrNot);
                                        BalancingCounter++;
                                    }
                                    if (FeasibleRebalanceOrNot == 1)
                                    {
                                        cout << "NewLogisticRatio: " << NewLogisticRatio << ", CurrentLogisticRatio: " << CurrentLogisticRatio << endl;
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
    cout << "Total # of solutions explored:" << solutionCounter << endl;
    cout << "Total # of solutions which are infeasible by balancing:" << CountingInfeasibleCase << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
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