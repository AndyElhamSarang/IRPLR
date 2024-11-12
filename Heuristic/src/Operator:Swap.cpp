#include "lib.h"
double solution_improvement::OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory)
{
    double violation = 0;
    double LR_objv = numeric_limits<double>::max();
    // IRPSolution.GetLogisticRatio(IRPLR);
    // cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    // LR_objv = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    cout << "LR objv:" << LR_objv << endl;
    solution Imp_Sol;
    int solutionCounter = 0;
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                for (int x = 0; x < IRPSolution.UnallocatedCustomers[i].size(); x++)
                {

                    solution tempIRP_sol(IRPSolution);
                    int tempCustomer = tempIRP_sol.Route[i][j][k];
                    tempIRP_sol.Route[i][j][k] = tempIRP_sol.UnallocatedCustomers[i][x];
                    tempIRP_sol.UnallocatedCustomers[i][x] = tempCustomer;
                    tempIRP_sol.VehicleLoad[i][j] = tempIRP_sol.VehicleLoad[i][j] - tempIRP_sol.DeliveryQuantity[tempIRP_sol.UnallocatedCustomers[i][x]][i]; // Customer gets unallocated, the vehicle load is reduced correspondingly
                    tempIRP_sol.DeliveryQuantity[tempIRP_sol.UnallocatedCustomers[i][x]][i] = 0;                                                             // Customer who become unallocated, the delivery quantity become 0;
                   
                    if (i == 0)
                    {
                        int tempInventory = IRPLR.Retailers[tempIRP_sol.UnallocatedCustomers[i][x]].InventoryBegin;
                        for (int y = 0; y < tempIRP_sol.InventoryLevel[tempIRP_sol.UnallocatedCustomers[i][x]].size(); y++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[tempIRP_sol.UnallocatedCustomers[i][x]].Demand + tempIRP_sol.DeliveryQuantity[tempIRP_sol.UnallocatedCustomers[i][x]][y];
                            tempIRP_sol.InventoryLevel[tempIRP_sol.UnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }
                    else
                    {

                        int tempInventory = tempIRP_sol.InventoryLevel[tempIRP_sol.UnallocatedCustomers[i][x]][i - 1];
                        for (int y = i; y < tempIRP_sol.InventoryLevel[tempIRP_sol.UnallocatedCustomers[i][x]].size(); y++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[tempIRP_sol.UnallocatedCustomers[i][x]].Demand + tempIRP_sol.DeliveryQuantity[tempIRP_sol.UnallocatedCustomers[i][x]][y];
                            tempIRP_sol.InventoryLevel[tempIRP_sol.UnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }

                    tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][i] = min(IRPLR.Vehicle.capacity - tempIRP_sol.VehicleLoad[i][j], IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax - tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]][i]);
                    //cout<<"!"<<IRPLR.Vehicle.capacity <<","<< tempIRP_sol.VehicleLoad[i][j]<<","<< IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax<<","<<tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]][i]<<","<<tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][i]<<endl;
                    tempIRP_sol.VehicleLoad[i][j] = tempIRP_sol.VehicleLoad[i][j] + tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][i];

                    if (i == 0)
                    {
                        double tempInventory = IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryBegin;
                        for (int y = 0; y < tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]].size(); y++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].Demand + tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y];
                            if (tempInventory > IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax)
                            {
                                tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y] = tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y] - (tempInventory - IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax);
                                tempInventory = IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax;
                            }
                            tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]][y] = tempInventory;
                        }
                    }
                    else
                    {

                        double tempInventory = tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]][i - 1];
                        for (int y = i; y < tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]].size(); y++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].Demand + tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y];
                            if (tempInventory > IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax)
                            {
                                tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y] = tempIRP_sol.DeliveryQuantity[tempIRP_sol.Route[i][j][k]][y] - (tempInventory - IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax);
                                tempInventory = IRPLR.Retailers[tempIRP_sol.Route[i][j][k]].InventoryMax;
                            }
                            tempIRP_sol.InventoryLevel[tempIRP_sol.Route[i][j][k]][y] = tempInventory;
                        }
                    }
                     cout << "Starting route:" << endl;
                    for (int y = 0; y < tempIRP_sol.Route[i][j].size(); y++)
                    {
                        cout << tempIRP_sol.Route[i][j][y] << ",";
                    }
                    cout << endl;
                    double CurrentTransportationCost = memory.PopulateSingleRoutePrefixAndSuffix(IRPLR, tempIRP_sol.Route[i][j]); // Preprocessing travel distance of the route
                    memory.PopulateSingleRouteSubpath(IRPLR, tempIRP_sol.Route[i][j]);
                    int FindingCheapestInsertion = OperatorCheapestInsertion(IRPLR, tempIRP_sol.Route[i][j], k, PenaltyForStockOut, CurrentTransportationCost, memory); // Finding cheapest insertion
                    cout << "Route resulting in cheapest insertion:" << endl;
                    for (int y = 0; y < tempIRP_sol.Route[i][j].size(); y++)
                    {
                        cout << tempIRP_sol.Route[i][j][y] << ",";
                    }
                    cout << endl;
                    /*for (int i = 0; i < tempIRP_sol.Route.size(); i++)
                    {
                        int NumberOfCustomerOfDay = 0;
                        for (int j = 0; j < tempIRP_sol.Route[i].size(); j++)
                        {
                            NumberOfCustomerOfDay += tempIRP_sol.Route[i][j].size();
                        }
                        if (NumberOfCustomerOfDay > 1)
                        {
                            tempIRP_sol.OutputCVRP(IRPLR, i, tempIRP_sol.Route[i]);
                            Routing.CallHGS(IRPLR);
                            tempIRP_sol.ReadCVRP_Solution(IRPLR, i, tempIRP_sol.Route[i]);
                        }
                    }*/
                    tempIRP_sol.GetLogisticRatio(IRPLR);
                    double temp_LR_obvj = tempIRP_sol.LogisticRatio + PenaltyForStockOut * tempIRP_sol.ViolationStockOut;

                    cout << "solution: " << solutionCounter << endl;
                    cout << "TotalTransportationCost:" << tempIRP_sol.TotalTransportationCost << "\t TotalDelivery:" << tempIRP_sol.TotalDelivery << "\t LogistcRatio:" << tempIRP_sol.LogisticRatio << endl;
                    cout << "ViolationStockOut" << tempIRP_sol.ViolationStockOut << "\t PenaltyForStockOut" << PenaltyForStockOut << "\t temp_LR_obvj:" << temp_LR_obvj << endl;
                    tempIRP_sol.print_solution(IRPLR);
                    if (temp_LR_obvj < LR_objv)
                    {
                        Imp_Sol = tempIRP_sol;
                        if (tempIRP_sol.ViolationStockOut > 0)
                        {
                            violation = tempIRP_sol.ViolationStockOut;
                        }
                        else
                        {
                            cout << "!Improving feasible solution found" << endl;
                        }

                        cout << "Improving solution found" << endl;
                    }

                    solutionCounter++;
                }
            }
        }
    }
    cout << "Total solution explored:" << solutionCounter << endl;

    IRPSolution = Imp_Sol;
    return violation;
}