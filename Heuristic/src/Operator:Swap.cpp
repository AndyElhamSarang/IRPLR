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

                    cout << "solution: " << solutionCounter << endl;
                    vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                    vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                    vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                    vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                    vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                    vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);

                    int tempCustomer = TempRoute[i][j][k];
                    TempRoute[i][j][k] = TempUnallocatedCustomers[i][x];
                    TempUnallocatedCustomers[i][x] = tempCustomer;

                    TempVehicleAllocation[TempRoute[i][j][k]][i] = TempVehicleAllocation[tempCustomer][i];
                    TempVehicleAllocation[tempCustomer][i] = IRPLR.NumberOfVehicles + 1;

                    TempVehicleLoad[i][j] = TempVehicleLoad[i][j] - TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][i]; // Customer gets unallocated, the vehicle load is reduced correspondingly
                    TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][i] = 0;
                    cout << "Remove a customer" << endl;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    if (i == 0)
                    {
                        int tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryBegin;
                        for (int y = 0; y < TempInventoryLevel[TempUnallocatedCustomers[i][x]].size(); y++)
                        {
                            if(TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]!=0)
                            {
                                //cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                               double DeltaQ= min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); //Compute how much you can delivery more
                               //cout<<"DeltaQ:"<<DeltaQ<<endl;
                               if(DeltaQ > 0)
                               {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] =  TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]+DeltaQ;
                               }

                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y];

                            if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax;
                            }

                            TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }
                    else
                    {

                        int tempInventory = TempInventoryLevel[TempUnallocatedCustomers[i][x]][i - 1];
                        for (int y = i; y < TempInventoryLevel[TempUnallocatedCustomers[i][x]].size(); y++)
                        {
                            if(TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]!=0)
                            {
                                //cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                               double DeltaQ= min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); //Compute how much you can delivery more
                               //cout<<"DeltaQ:"<<DeltaQ<<endl;
                               if(DeltaQ > 0)
                               {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] =  TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]+DeltaQ;
                               }

                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y];

                            if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax;
                            }

                            TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }
                    cout << "Adjusted the inventory level of the removed customer" << endl;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    TempDeliveryQuantity[TempRoute[i][j][k]][i] = min(IRPLR.Vehicle.capacity - TempVehicleLoad[i][j], IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax - TempInventoryLevel[TempRoute[i][j][k]][i]);
                    // cout<<"!"<<IRPLR.Vehicle.capacity <<","<< TempVehicleLoad[i][j]<<","<< IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax<<","<<TempInventoryLevel[TempRoute[i][j][k]][i]<<","<<TempDeliveryQuantity[TempRoute[i][j][k]][i]<<endl;
                    TempVehicleLoad[i][j] = TempVehicleLoad[i][j] + TempDeliveryQuantity[TempRoute[i][j][k]][i];
                    cout << "Added delivery quantity to the inserted customer" << endl;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    if (i == 0)
                    {
                        double tempInventory = IRPLR.Retailers[TempRoute[i][j][k]].InventoryBegin;
                        for (int y = 0; y < TempInventoryLevel[TempRoute[i][j][k]].size(); y++)
                        {
                            if(TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]!=0)
                            {
                                //cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                               double DeltaQ= min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); //Compute how much you can delivery more
                               //cout<<"DeltaQ:"<<DeltaQ<<endl;
                               if(DeltaQ > 0)
                               {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] =  TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]+DeltaQ;
                               }

                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempRoute[i][j][k]].Demand + TempDeliveryQuantity[TempRoute[i][j][k]][y];
                            if (tempInventory > IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] - (tempInventory - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax;
                            }
                            TempInventoryLevel[TempRoute[i][j][k]][y] = tempInventory;
                        }
                    }
                    else
                    {

                        double tempInventory = TempInventoryLevel[TempRoute[i][j][k]][i - 1];
                        for (int y = i; y < TempInventoryLevel[TempRoute[i][j][k]].size(); y++)
                        {
                            if(TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]!=0)
                            {
                                //cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                               double DeltaQ= min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); //Compute how much you can delivery more
                               //cout<<"DeltaQ:"<<DeltaQ<<endl;
                               if(DeltaQ > 0)
                               {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] =  TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y]+DeltaQ;
                               }

                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempRoute[i][j][k]].Demand + TempDeliveryQuantity[TempRoute[i][j][k]][y];
                            if (tempInventory > IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] - (tempInventory - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax;
                            }
                            TempInventoryLevel[TempRoute[i][j][k]][y] = tempInventory;
                        }
                    }
                    cout << "Adjusted inventory level to the inserted customer" << endl;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    cout << "Starting route:" << endl;
                    for (int y = 0; y < TempRoute[i][j].size(); y++)
                    {
                        cout << TempRoute[i][j][y] << ",";
                    }
                    cout << endl;
                    double CurrentTransportationCost = memory.PopulateSingleRoutePrefixAndSuffix(IRPLR, TempRoute[i][j]); // Preprocessing travel distance of the route
                    memory.PopulateSingleRouteSubpath(IRPLR, TempRoute[i][j]);
                    
                    int FindingCheapestInsertion = OperatorCheapestInsertion(IRPLR, TempRoute[i][j], k,  PenaltyForStockOut, CurrentTransportationCost, memory); // Finding cheapest insertion
                    cout << "Route resulting in cheapest insertion:" << endl;
                    for (int y = 0; y < TempRoute[i][j].size(); y++)
                    {
                        cout << TempRoute[i][j][y] << ",";
                    }
                    cout << endl;

                   
                    // tempIRP_sol.GetLogisticRatio(IRPLR);
                    // double temp_LR_obvj = tempIRP_sol.LogisticRatio + PenaltyForStockOut * tempIRP_sol.ViolationStockOut;

                    // cout << "TotalTransportationCost:" << tempIRP_sol.TotalTransportationCost << "\t TotalDelivery:" << tempIRP_sol.TotalDelivery << "\t LogistcRatio:" << tempIRP_sol.LogisticRatio << endl;
                    // cout << "ViolationStockOut" << tempIRP_sol.ViolationStockOut << "\t PenaltyForStockOut" << PenaltyForStockOut << "\t temp_LR_obvj:" << temp_LR_obvj << endl;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                    /*if (temp_LR_obvj < LR_objv)
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
                    }*/

                    solutionCounter++;
                }
            }
        }
    }
    cout << "Total solution explored:" << solutionCounter << endl;

    IRPSolution = Imp_Sol;
    return violation;
}