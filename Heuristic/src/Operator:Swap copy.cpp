#include "lib.h"
int solution_improvement::OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory)
{
    cout << "Swap starting solution" << endl;
    int whether_improved_or_not = 0;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    LR_objv = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    cout << "LR objv:" << LR_objv << endl;

    vector<vector<vector<int>>> ImpRoute(IRPSolution.Route);
    vector<vector<int>> ImpUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
    vector<vector<double>> ImpVehicleLoad(IRPSolution.VehicleLoad);
    vector<vector<double>> ImpDeliveryQuantity(IRPSolution.DeliveryQuantity);
    vector<vector<double>> ImpInventoryLevel(IRPSolution.InventoryLevel);
    vector<vector<int>> ImpVehicleAllocation(IRPSolution.VehicleAllocation);
    vector<vector<int>> ImpVisitOrder(IRPSolution.VisitOrder);
    double ImpStockOut = 0;
    int solutionCounter = 0;
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                for (int x = 0; x < IRPSolution.UnallocatedCustomers[i].size(); x++)
                {
                    double ChangeInTransportationCost = 0;
                    double ChangeInTotalQuantity = 0;
                    double NewStockOut = IRPSolution.ViolationStockOut;
                    // cout << "solution: " << solutionCounter << endl;
                    vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                    vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                    vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                    vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                    vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                    vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);
                    vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);
                    // double testEndTransportationCost = 0; 
                    // double testStartTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                    // testStartTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                    // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                    //{
                    //     testStartTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                    // }

                    int tempCustomer = TempRoute[i][j][k];
                    TempRoute[i][j][k] = TempUnallocatedCustomers[i][x];
                    TempUnallocatedCustomers[i][x] = tempCustomer;
                    if (k == 0)
                    {
                        // cout << "From depot" << endl;
                        if (TempRoute[i][j].size() <= 1)
                        {
                            // cout << "Route with single customer" << endl;
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                            // cout << "ChangeInTransportationCost:" << ChangeInTransportationCost << endl;

                            // testEndTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                            // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                            // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                            //{
                            //  testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                            //}
                            // cout << "testEndTransportationCost:" << testEndTransportationCost << ", testStartTransportationCost:" << testStartTransportationCost << ", ChangeInTransportationCost:" << ChangeInTransportationCost << endl;
                            // assert(fabs(testEndTransportationCost - testStartTransportationCost) - fabs(ChangeInTransportationCost) < 0.001);
                        }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                            // cout << "ChangeInTransportationCost:" << ChangeInTransportationCost << endl;

                            // testEndTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                            // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                            // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                            //{
                            //   testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                            // }
                            // cout << "testEndTransportationCost:" << testEndTransportationCost << ", testStartTransportationCost:" << testStartTransportationCost << ", ChangeInTransportationCost:" << ChangeInTransportationCost << endl;
                            // assert(fabs(testEndTransportationCost - testStartTransportationCost) - fabs(ChangeInTransportationCost) < 0.001);
                        }
                    }
                    else if (k == IRPSolution.Route[i][j].size() - 1)
                    {
                        // cout << "Back to depot" << endl;
                        if (TempRoute[i][j].size() <= 1)
                        {
                            // cout << "Route with single customer" << endl;
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                            // cout << "ChangeInTransportationCost:" << ChangeInTransportationCost << endl;

                            // testEndTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                            // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                            // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                            //{
                            // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                            //}
                            // cout << "testEndTransportationCost:" << testEndTransportationCost << ", testStartTransportationCost:" << testStartTransportationCost << ", ChangeInTransportationCost:" << ChangeInTransportationCost << endl;
                            // assert(fabs(testEndTransportationCost - testStartTransportationCost) - fabs(ChangeInTransportationCost) < 0.001);
                        }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                            // cout << "ChangeInTransportationCost:" << ChangeInTransportationCost << endl;

                            // testEndTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                            // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                            // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                            //{
                            //     testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                            // }
                            // cout << "testEndTransportationCost:" << testEndTransportationCost << ", testStartTransportationCost:" << testStartTransportationCost << ", ChangeInTransportationCost:" << ChangeInTransportationCost << endl;
                            // assert(fabs(testEndTransportationCost - testStartTransportationCost) - fabs(ChangeInTransportationCost) < 0.001);
                        }
                    }
                    else
                    {
                        // cout << "In the middle of the route" << endl;
                        ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                        // cout << "ChangeInTransportationCost:" << ChangeInTransportationCost << endl;

                        // testEndTransportationCost = IRPLR.Distance[0][TempRoute[i][j][0] + 1];
                        // testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][TempRoute[i][j].size() - 1] + 1][0];
                        // for (int test = 0; test < TempRoute[i][j].size() - 1; test++)
                        //{
                        //    testEndTransportationCost += IRPLR.Distance[TempRoute[i][j][test] + 1][TempRoute[i][j][test + 1] + 1];
                        // }
                        // cout << "testEndTransportationCost:" << testEndTransportationCost << ", testStartTransportationCost:" << testStartTransportationCost << ", ChangeInTransportationCost:" << ChangeInTransportationCost << endl;
                        // assert(fabs(testEndTransportationCost - testStartTransportationCost) - fabs(ChangeInTransportationCost) < 0.001);
                    }

                    TempVehicleAllocation[TempRoute[i][j][k]][i] = TempVehicleAllocation[tempCustomer][i];
                    TempVisitOrder[TempRoute[i][j][k]][i] = TempVisitOrder[tempCustomer][i];
                    TempVehicleAllocation[tempCustomer][i] = IRPLR.NumberOfVehicles + 1;
                    TempVisitOrder[tempCustomer][i] = IRPLR.Retailers.size() + 1;


                    TempVehicleLoad[i][j] = TempVehicleLoad[i][j] - TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][i]; // Customer gets unallocated, the vehicle load is reduced correspondingly
                    ChangeInTotalQuantity = 0 - TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][i];
                    TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][i] = 0;
                    // cout << "Remove a customer" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    if (i == 0)
                    {
                        double tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryBegin;
                        for (int y = 0; y < TempInventoryLevel[TempUnallocatedCustomers[i][x]].size(); y++)
                        {
                            if (TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] != 0)
                            {
                                // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                                double DeltaQ = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); // Compute how much you can delivery more
                                // cout<<"DeltaQ:"<<DeltaQ<<endl;
                                if (DeltaQ > 0)
                                {
                                    TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] + DeltaQ;
                                    ChangeInTotalQuantity += DeltaQ;
                                    TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] + DeltaQ;
                                }
                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y];

                            if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                ChangeInTotalQuantity = ChangeInTotalQuantity - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax;

                                
                            }
                            if (fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory) > 0.001)
                            {
                                if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] >= 0.0 && tempInventory < -0.001)
                                {
                                    NewStockOut += -tempInventory;
                                }
                                else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < -0.001 && tempInventory >= 0.0)
                                {
                                    NewStockOut -= -TempInventoryLevel[TempUnallocatedCustomers[i][x]][y];
                                }
                                else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < -0.001 && tempInventory < -0.001)
                                {
                                    if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < tempInventory)
                                    {
                                        NewStockOut -= fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory);
                                    }
                                    else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] > tempInventory)
                                    {
                                        NewStockOut += fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory);
                                    }
                                }
                            }
                            TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }
                    else
                    {

                        double tempInventory = TempInventoryLevel[TempUnallocatedCustomers[i][x]][i - 1];
                        for (int y = i; y < TempInventoryLevel[TempUnallocatedCustomers[i][x]].size(); y++)
                        {
                            if (TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] != 0)
                            {
                                // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                                double DeltaQ = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]], IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]); // Compute how much you can delivery more
                                // cout<<"DeltaQ:"<<DeltaQ<<endl;
                                if (DeltaQ > 0)
                                {
                                    TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] + DeltaQ;
                                    ChangeInTotalQuantity += DeltaQ;
                                    TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                }
                            }
                            tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y];

                            if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[i][x]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                ChangeInTotalQuantity = ChangeInTotalQuantity - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax);
                                tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax;

                                
                            }

                            if (fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory) > 0.001)
                            {
                                if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] >= 0.0 && tempInventory < -0.001)
                                {
                                    NewStockOut += -tempInventory;
                                }
                                else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < -0.001 && tempInventory >= 0.0)
                                {
                                    NewStockOut -= -TempInventoryLevel[TempUnallocatedCustomers[i][x]][y];
                                }
                                else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < -0.001 && tempInventory < -0.001)
                                {
                                    if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] < tempInventory)
                                    {
                                        NewStockOut -= fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory);
                                    }
                                    else if (TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] > tempInventory)
                                    {
                                        NewStockOut += fabs(TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] - tempInventory);
                                    }
                                }
                            }

                            TempInventoryLevel[TempUnallocatedCustomers[i][x]][y] = tempInventory;
                        }
                    }
                    // cout << "Adjusted the inventory level of the removed customer" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                    double CopyOfQ = TempDeliveryQuantity[TempRoute[i][j][k]][i]; // Should always be zero at this line
                    TempDeliveryQuantity[TempRoute[i][j][k]][i] = min(IRPLR.Vehicle.capacity - TempVehicleLoad[i][j], IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax - TempInventoryLevel[TempRoute[i][j][k]][i]);
                    ChangeInTotalQuantity += TempDeliveryQuantity[TempRoute[i][j][k]][i] - CopyOfQ;

                    // cout<<"!"<<IRPLR.Vehicle.capacity <<","<< TempVehicleLoad[i][j]<<","<< IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax<<","<<TempInventoryLevel[TempRoute[i][j][k]][i]<<","<<TempDeliveryQuantity[TempRoute[i][j][k]][i]<<endl;
                    TempVehicleLoad[i][j] = TempVehicleLoad[i][j] + TempDeliveryQuantity[TempRoute[i][j][k]][i];
                    // cout << "Added delivery quantity to the inserted customer" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    if (i == 0)
                    {
                        double AdjustInventoryInsert = IRPLR.Retailers[TempRoute[i][j][k]].InventoryBegin;
                        for (int y = 0; y < TempInventoryLevel[TempRoute[i][j][k]].size(); y++)
                        {
                            if (TempDeliveryQuantity[TempRoute[i][j][k]][y] != 0)
                            {
                                // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                                double DeltaQ_Insert = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]], IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax - TempInventoryLevel[TempRoute[i][j][k]][y]); // Compute how much you can delivery more
                                // cout<<"DeltaQ_Insert:"<<DeltaQ_Insert<<endl;
                                if (DeltaQ_Insert > 0)
                                {
                                    TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] + DeltaQ_Insert;
                                    ChangeInTotalQuantity += DeltaQ_Insert;
                                    TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] + DeltaQ_Insert;
                                }
                            }
                            AdjustInventoryInsert = AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].Demand + TempDeliveryQuantity[TempRoute[i][j][k]][y];
                            if (AdjustInventoryInsert > IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                ChangeInTotalQuantity = ChangeInTotalQuantity - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                AdjustInventoryInsert = IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax;

                                
                            }

                            if (fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert) > 0.001)
                            {
                                if (TempInventoryLevel[TempRoute[i][j][k]][y] >= 0.0 && AdjustInventoryInsert < -0.001)
                                {
                                    NewStockOut += -AdjustInventoryInsert;
                                }
                                else if (TempInventoryLevel[TempRoute[i][j][k]][y] < -0.001 && AdjustInventoryInsert >= 0.0)
                                {
                                    NewStockOut -= -TempInventoryLevel[TempRoute[i][j][k]][y];
                                }
                                else if (TempInventoryLevel[TempRoute[i][j][k]][y] < -0.001 && AdjustInventoryInsert < -0.001)
                                {
                                    if (TempInventoryLevel[TempRoute[i][j][k]][y] < AdjustInventoryInsert)
                                    {
                                        NewStockOut -= fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert);
                                    }
                                    else if (TempInventoryLevel[TempRoute[i][j][k]][y] > AdjustInventoryInsert)
                                    {
                                        NewStockOut += fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert);
                                    }
                                }
                            }

                            TempInventoryLevel[TempRoute[i][j][k]][y] = AdjustInventoryInsert;
                        }
                    }
                    else
                    {

                        double AdjustInventoryInsert = TempInventoryLevel[TempRoute[i][j][k]][i - 1];
                        for (int y = i; y < TempInventoryLevel[TempRoute[i][j][k]].size(); y++)
                        {
                            if (TempDeliveryQuantity[TempRoute[i][j][k]][y] != 0)
                            {
                                // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[i][x]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[i][x]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[i][x]][y]<<endl;
                                double DeltaQ_Insert = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]], IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax - TempInventoryLevel[TempRoute[i][j][k]][y]); // Compute how much you can delivery more
                                // cout<<"DeltaQ_Insert:"<<DeltaQ_Insert<<endl;
                                if (DeltaQ_Insert > 0)
                                {
                                    TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] + DeltaQ_Insert;
                                    ChangeInTotalQuantity += DeltaQ_Insert;
                                    TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] + DeltaQ_Insert;
                                }
                            }
                            AdjustInventoryInsert = AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].Demand + TempDeliveryQuantity[TempRoute[i][j][k]][y];
                            if (AdjustInventoryInsert > IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax)
                            {
                                TempDeliveryQuantity[TempRoute[i][j][k]][y] = TempDeliveryQuantity[TempRoute[i][j][k]][y] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                ChangeInTotalQuantity = ChangeInTotalQuantity - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[i][j][k]][y]] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax);
                                AdjustInventoryInsert = IRPLR.Retailers[TempRoute[i][j][k]].InventoryMax;

                                
                            }

                            if (fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert) > 0.001)
                            {
                                if (TempInventoryLevel[TempRoute[i][j][k]][y] >= 0.0 && AdjustInventoryInsert < -0.001)
                                {
                                    NewStockOut += -AdjustInventoryInsert;
                                }
                                else if (TempInventoryLevel[TempRoute[i][j][k]][y] < -0.001 && AdjustInventoryInsert >= 0.0)
                                {
                                    NewStockOut -= -TempInventoryLevel[TempRoute[i][j][k]][y];
                                }
                                else if (TempInventoryLevel[TempRoute[i][j][k]][y] < -0.001 && AdjustInventoryInsert < -0.001)
                                {
                                    if (TempInventoryLevel[TempRoute[i][j][k]][y] < AdjustInventoryInsert)
                                    {
                                        NewStockOut -= fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert);
                                    }
                                    else if (TempInventoryLevel[TempRoute[i][j][k]][y] > AdjustInventoryInsert)
                                    {
                                        NewStockOut += fabs(TempInventoryLevel[TempRoute[i][j][k]][y] - AdjustInventoryInsert);
                                    }
                                }
                            }
                            TempInventoryLevel[TempRoute[i][j][k]][y] = AdjustInventoryInsert;
                        }
                    }
                    // cout << "Adjusted inventory level to the inserted customer" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    // cout << "Starting route:" << endl;
                    // for (int y = 0; y < TempRoute[i][j].size(); y++)
                    //{
                    // cout << TempRoute[i][j][y] << ",";
                    // }
                    // cout << endl;

                    double CurrentTransportationCost = memory.PopulateSingleRoutePrefixAndSuffix(IRPLR, TempRoute[i][j]); // Preprocessing travel distance of the route
                                                                                                                          // cout << testEndTransportationCost << "," << CurrentTransportationCost << endl;
                    // assert(fabs(testEndTransportationCost - CurrentTransportationCost) < 0.001);
                    memory.PopulateSingleRouteSubpath(IRPLR, TempRoute[i][j]);
                    double ImprovedTransportationCost = CurrentTransportationCost;
                    int FindingCheapestInsertion = OperatorCheapestInsertion(IRPLR, TempRoute[i][j], k, PenaltyForStockOut, ImprovedTransportationCost, memory); // Finding cheapest insertion
                                                                                                                                                                 // cout << CurrentTransportationCost << "," << ImprovedTransportationCost << "," << CurrentTransportationCost - ImprovedTransportationCost << endl;
                    ChangeInTransportationCost += ImprovedTransportationCost - CurrentTransportationCost;
                    // cout << ChangeInTransportationCost << endl;
                    double NewTransportationCost = IRPSolution.TotalTransportationCost + ChangeInTransportationCost;
                    double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;

                    // cout << "Route resulting in cheapest insertion:" << endl;
                    // for (int y = 0; y < TempRoute[i][j].size(); y++)
                    //{
                    //     cout << TempRoute[i][j][y] << ",";
                    // }
                    // cout << endl;

                    double NewLogisticRatio = NewTransportationCost / NewTotalDelivery;

                    double temp_LR_obvj = NewLogisticRatio + PenaltyForStockOut * NewStockOut;

                    // double TempTransportationCost = 0;
                    // double TempTotalDelivery = 0;
                    // double TempLogisticRatio = 0;
                    // double TempViolationStockOut = 0;
                    // GetTempLogisticRatio(IRPLR, TempRoute, TempDeliveryQuantity, TempInventoryLevel, TempTransportationCost, TempTotalDelivery, TempLogisticRatio, TempViolationStockOut);
                    // cout << "Final solution" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                    // cout << "Check Transportation Cost" << endl;
                    // cout << TempTransportationCost << "," << NewTransportationCost << endl;
                    // assert(fabs(TempTransportationCost - NewTransportationCost) < 0.001);
                    // cout << "Check Delivery Quantity" << endl;
                    // cout << TempTotalDelivery << "," << NewTotalDelivery << endl;
                    // assert(fabs(TempTotalDelivery - NewTotalDelivery) < 0.001);
                    // cout << "Check Logistic ratio" << endl;
                    // cout << TempLogisticRatio << "," << NewLogisticRatio << endl;
                    // assert(fabs(TempLogisticRatio - NewLogisticRatio) < 0.001);
                    // cout << "Check ViolationStockout" << endl;
                    // cout << TempViolationStockOut << "," << NewStockOut << endl;
                    // assert(fabs(TempViolationStockOut - NewStockOut) < 0.001);
                    //  cout << temp_LR_obvj << "," << LR_objv << endl;
                    if (temp_LR_obvj < LR_objv)
                    {
                        whether_improved_or_not = 1;
                        ImpRoute = TempRoute;
                        ImpInventoryLevel = TempInventoryLevel;
                        ImpDeliveryQuantity = TempDeliveryQuantity;
                        ImpUnallocatedCustomers = TempUnallocatedCustomers;
                        ImpVehicleAllocation = TempVehicleAllocation;
                        ImpVehicleLoad = TempVehicleLoad;
                        ImpVisitOrder = TempVisitOrder;
                        if (NewStockOut > 0)
                        {
                            ImpStockOut = NewStockOut;
                        }
                        else
                        {
                            ImpStockOut = 0;
                            // cout << "!Improving feasible solution found" << endl;
                        }

                        // cout << "!Improving solution found" << endl;
                    }

                    solutionCounter++;
                }
            }
        }
    }
    cout << "Total solution explored:" << solutionCounter << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    if (whether_improved_or_not == 1)
    {
        IRPSolution.Route = ImpRoute;
        IRPSolution.InventoryLevel = ImpInventoryLevel;
        IRPSolution.DeliveryQuantity = ImpDeliveryQuantity;
        IRPSolution.UnallocatedCustomers = ImpUnallocatedCustomers;
        IRPSolution.VehicleAllocation = ImpVehicleAllocation;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.VisitOrder = ImpVisitOrder;
        IRPSolution.ViolationStockOut = ImpStockOut;

        IRPSolution.GetLogisticRatio(IRPLR);
        assert(fabs(IRPSolution.ViolationStockOut - ImpStockOut) < 0.001);

        //Update the visit order
        for (int i = 0; i < IRPSolution.VisitOrder.size(); i++)
        {
            for (int j = 0; j < IRPSolution.VisitOrder[i].size(); j++)
            {
                IRPSolution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
                IRPSolution.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
            }
        }

        IRPSolution.UnallocatedCustomers.clear();
        for (int i = 0; i < IRPSolution.Route.size(); i++) // For this time period
        {
            vector<int> TempUnallocatedCustomer;             // Look for unallcated customers at this time period
            for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
            {
                int UnallocatedYesOrNo = 0;
                for (int j = 0; j < IRPSolution.Route[i].size(); j++) // index j for vehicle
                {
                    for (int k = 0; k < IRPSolution.Route[i][j].size(); k++) // index k for position
                    {

                        if (IRPSolution.Route[i][j][k] == x)
                        {
                            UnallocatedYesOrNo = 1;
                            IRPSolution.VehicleAllocation[x][i] = j;
                            IRPSolution.VisitOrder[x][i] = k;
                        }
                    }
                }
                if (UnallocatedYesOrNo == 0) // This customer is not visited
                {
                    TempUnallocatedCustomer.push_back(x);
                }
            }
            IRPSolution.UnallocatedCustomers.push_back(TempUnallocatedCustomer);
        }

        cout<<"Solution before rebalancing"<<endl;
        IRPSolution.print_solution(IRPLR);

        
        // IRPSolution.GetLogisticRatio(IRPLR);
        // cout << IRPSolution.ViolationStockOut << "," << violation << endl;
        // assert(fabs(IRPSolution.ViolationStockOut - violation) < 0.001);
    }

    return whether_improved_or_not;
}