#include "lib.h"
int solution_improvement::OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory,
                                       int &min_remove_length, int &max_remove_length,
                                       int &min_insert_length, int &max_insert_length)
{

    //Current implementation only permits at most one customer therefore
    assert(max_remove_length <=1);
    assert(max_insert_length <= 1);

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
    for (int pick_day = 0; pick_day < IRPSolution.Route.size(); pick_day++) // For a day
    {
        for (int pick_vehicle = 0; pick_vehicle < IRPSolution.Route[pick_day].size(); pick_vehicle++) // For a vehicle
        {
            for (int pick_allocated_customer = 0; pick_allocated_customer < IRPSolution.Route[pick_day][pick_vehicle].size(); pick_allocated_customer++) // A position in this vehicle
            {
                for (int pick_unallocated_customer = 0; pick_unallocated_customer < IRPSolution.UnallocatedCustomers[pick_day].size(); pick_unallocated_customer++) // A position in the unallocated customer
                {
                    for (int remove_length = min_remove_length; remove_length <= max_remove_length; remove_length++)
                    {
                        for (int insert_length = min_insert_length; insert_length <= max_insert_length; insert_length++)
                        {
                            if (remove_length != 0 || insert_length != 0)
                            {
                                if (remove_length != 0 && insert_length == 0 && pick_unallocated_customer > 0)
                                {
                                    // Do nothing if it is just a remove when pick_unallocated_customer > 0
                                }
                                else
                                {
                                    if (pick_unallocated_customer + insert_length <= IRPSolution.UnallocatedCustomers[pick_day].size() && pick_allocated_customer + remove_length <= IRPSolution.Route[pick_day][pick_vehicle].size()) // Make sure the operator does not go outside the range
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

                                        PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder);
                                        
                                        //Calcualte the new transportation cost
                                        double tempTransportationCost = IRPLR.Distance[0][TempRoute[pick_day][pick_vehicle][0] + 1];
                                        tempTransportationCost += IRPLR.Distance[TempRoute[pick_day][pick_vehicle][TempRoute[pick_day][pick_vehicle].size() - 1] + 1][0];
                                        for (int test = 0; test < TempRoute[pick_day][pick_vehicle].size() - 1; test++)
                                        {
                                            tempTransportationCost += IRPLR.Distance[TempRoute[pick_day][pick_vehicle][test] + 1][TempRoute[pick_day][pick_vehicle][test + 1] + 1];
                                        }
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

                                        TempVehicleLoad[pick_day][pick_vehicle] = TempVehicleLoad[pick_day][pick_vehicle] - TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][pick_day]; // Customer gets unallocated, the vehicle load is reduced correspondingly
                                        ChangeInTotalQuantity = 0 - TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][pick_day];
                                        TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][pick_day] = 0;
                                        // cout << "Remove a customer" << endl;
                                        // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                                        if (pick_day == 0)
                                        {
                                            double tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryBegin;
                                            for (int y = 0; y < TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].size(); y++)
                                            {
                                                if (TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] != 0)
                                                {
                                                    // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]<<endl;
                                                    double DeltaQ = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]], IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]); // Compute how much you can delivery more
                                                    // cout<<"DeltaQ:"<<DeltaQ<<endl;
                                                    if (DeltaQ > 0)
                                                    {
                                                        TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] + DeltaQ;
                                                        ChangeInTotalQuantity += DeltaQ;
                                                        TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] + DeltaQ;
                                                    }
                                                }
                                                tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y];

                                                if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax)
                                                {
                                                    TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    ChangeInTotalQuantity = ChangeInTotalQuantity - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax;
                                                }
                                                if (fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory) > 0.001)
                                                {
                                                    if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] >= 0.0 && tempInventory < -0.001)
                                                    {
                                                        NewStockOut += -tempInventory;
                                                    }
                                                    else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < -0.001 && tempInventory >= 0.0)
                                                    {
                                                        NewStockOut -= -TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y];
                                                    }
                                                    else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < -0.001 && tempInventory < -0.001)
                                                    {
                                                        if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < tempInventory)
                                                        {
                                                            NewStockOut -= fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory);
                                                        }
                                                        else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] > tempInventory)
                                                        {
                                                            NewStockOut += fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory);
                                                        }
                                                    }
                                                }
                                                TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = tempInventory;
                                            }
                                        }
                                        else
                                        {

                                            double tempInventory = TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][pick_day - 1];
                                            for (int y = pick_day; y < TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].size(); y++)
                                            {
                                                if (TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] != 0)
                                                {
                                                    // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]<<endl;
                                                    double DeltaQ = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]], IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]); // Compute how much you can delivery more
                                                    // cout<<"DeltaQ:"<<DeltaQ<<endl;
                                                    if (DeltaQ > 0)
                                                    {
                                                        TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] + DeltaQ;
                                                        ChangeInTotalQuantity += DeltaQ;
                                                        TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    }
                                                }
                                                tempInventory = tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].Demand + TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y];

                                                if (tempInventory > IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax)
                                                {
                                                    TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = TempDeliveryQuantity[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    ChangeInTotalQuantity = ChangeInTotalQuantity - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]] - (tempInventory - IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax);
                                                    tempInventory = IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax;
                                                }

                                                if (fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory) > 0.001)
                                                {
                                                    if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] >= 0.0 && tempInventory < -0.001)
                                                    {
                                                        NewStockOut += -tempInventory;
                                                    }
                                                    else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < -0.001 && tempInventory >= 0.0)
                                                    {
                                                        NewStockOut -= -TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y];
                                                    }
                                                    else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < -0.001 && tempInventory < -0.001)
                                                    {
                                                        if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] < tempInventory)
                                                        {
                                                            NewStockOut -= fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory);
                                                        }
                                                        else if (TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] > tempInventory)
                                                        {
                                                            NewStockOut += fabs(TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] - tempInventory);
                                                        }
                                                    }
                                                }

                                                TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y] = tempInventory;
                                            }
                                        }
                                        // cout << "Adjusted the inventory level of the removed customer" << endl;
                                        // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                                        double CopyOfQ = TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day]; // Should always be zero at this line
                                        TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day] = min(IRPLR.Vehicle.capacity - TempVehicleLoad[pick_day][pick_vehicle], IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax - TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day]);
                                        ChangeInTotalQuantity += TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day] - CopyOfQ;

                                        // cout<<"!"<<IRPLR.Vehicle.capacity <<","<< TempVehicleLoad[pick_day][pick_vehicle]<<","<< IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax<<","<<TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day]<<","<<TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day]<<endl;
                                        TempVehicleLoad[pick_day][pick_vehicle] = TempVehicleLoad[pick_day][pick_vehicle] + TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day];
                                        // cout << "Added delivery quantity to the inserted customer" << endl;
                                        // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                                        if (pick_day == 0)
                                        {
                                            double AdjustInventoryInsert = IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryBegin;
                                            for (int y = 0; y < TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].size(); y++)
                                            {
                                                if (TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] != 0)
                                                {
                                                    // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]<<endl;
                                                    double DeltaQ_Insert = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]], IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax - TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]); // Compute how much you can delivery more
                                                    // cout<<"DeltaQ_Insert:"<<DeltaQ_Insert<<endl;
                                                    if (DeltaQ_Insert > 0)
                                                    {
                                                        TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] + DeltaQ_Insert;
                                                        ChangeInTotalQuantity += DeltaQ_Insert;
                                                        TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] + DeltaQ_Insert;
                                                    }
                                                }
                                                AdjustInventoryInsert = AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].Demand + TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y];
                                                if (AdjustInventoryInsert > IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax)
                                                {
                                                    TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    ChangeInTotalQuantity = ChangeInTotalQuantity - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    AdjustInventoryInsert = IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax;
                                                }

                                                if (fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert) > 0.001)
                                                {
                                                    if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] >= 0.0 && AdjustInventoryInsert < -0.001)
                                                    {
                                                        NewStockOut += -AdjustInventoryInsert;
                                                    }
                                                    else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < -0.001 && AdjustInventoryInsert >= 0.0)
                                                    {
                                                        NewStockOut -= -TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y];
                                                    }
                                                    else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < -0.001 && AdjustInventoryInsert < -0.001)
                                                    {
                                                        if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < AdjustInventoryInsert)
                                                        {
                                                            NewStockOut -= fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert);
                                                        }
                                                        else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] > AdjustInventoryInsert)
                                                        {
                                                            NewStockOut += fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert);
                                                        }
                                                    }
                                                }

                                                TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = AdjustInventoryInsert;
                                            }
                                        }
                                        else
                                        {

                                            double AdjustInventoryInsert = TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][pick_day - 1];
                                            for (int y = pick_day; y < TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].size(); y++)
                                            {
                                                if (TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] != 0)
                                                {
                                                    // cout<<IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]]<<","<< IRPLR.Retailers[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]].InventoryMax - TempInventoryLevel[TempUnallocatedCustomers[pick_day][pick_unallocated_customer]][y]<<endl;
                                                    double DeltaQ_Insert = min(IRPLR.Vehicle.capacity - TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]], IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax - TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]); // Compute how much you can delivery more
                                                    // cout<<"DeltaQ_Insert:"<<DeltaQ_Insert<<endl;
                                                    if (DeltaQ_Insert > 0)
                                                    {
                                                        TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] + DeltaQ_Insert;
                                                        ChangeInTotalQuantity += DeltaQ_Insert;
                                                        TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] + DeltaQ_Insert;
                                                    }
                                                }
                                                AdjustInventoryInsert = AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].Demand + TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y];
                                                if (AdjustInventoryInsert > IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax)
                                                {
                                                    TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = TempDeliveryQuantity[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    ChangeInTotalQuantity = ChangeInTotalQuantity - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] = TempVehicleLoad[y][TempVehicleAllocation[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y]] - (AdjustInventoryInsert - IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax);
                                                    AdjustInventoryInsert = IRPLR.Retailers[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]].InventoryMax;
                                                }

                                                if (fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert) > 0.001)
                                                {
                                                    if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] >= 0.0 && AdjustInventoryInsert < -0.001)
                                                    {
                                                        NewStockOut += -AdjustInventoryInsert;
                                                    }
                                                    else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < -0.001 && AdjustInventoryInsert >= 0.0)
                                                    {
                                                        NewStockOut -= -TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y];
                                                    }
                                                    else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < -0.001 && AdjustInventoryInsert < -0.001)
                                                    {
                                                        if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] < AdjustInventoryInsert)
                                                        {
                                                            NewStockOut -= fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert);
                                                        }
                                                        else if (TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] > AdjustInventoryInsert)
                                                        {
                                                            NewStockOut += fabs(TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] - AdjustInventoryInsert);
                                                        }
                                                    }
                                                }
                                                TempInventoryLevel[TempRoute[pick_day][pick_vehicle][pick_allocated_customer]][y] = AdjustInventoryInsert;
                                            }
                                        }
                                        // cout << "Adjusted inventory level to the inserted customer" << endl;
                                        // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                                        // cout << "Starting route:" << endl;
                                        // for (int y = 0; y < TempRoute[pick_day][pick_vehicle].size(); y++)
                                        //{
                                        // cout << TempRoute[pick_day][pick_vehicle][y] << ",";
                                        // }
                                        // cout << endl;

                                        double CurrentTransportationCost = memory.PopulateSingleRoutePrefixAndSuffix(IRPLR, TempRoute[pick_day][pick_vehicle]); // Preprocessing travel distance of the route
                                                                                                                                                                // cout << testEndTransportationCost << "," << CurrentTransportationCost << endl;
                                        // assert(fabs(testEndTransportationCost - CurrentTransportationCost) < 0.001);
                                        memory.PopulateSingleRouteSubpath(IRPLR, TempRoute[pick_day][pick_vehicle]);
                                        double ImprovedTransportationCost = CurrentTransportationCost;
                                        int FindingCheapestInsertion = OperatorCheapestInsertion(IRPLR, TempRoute[pick_day][pick_vehicle], pick_allocated_customer, PenaltyForStockOut, ImprovedTransportationCost, memory); // Finding cheapest insertion
                                                                                                                                                                                                                             // cout << CurrentTransportationCost << "," << ImprovedTransportationCost << "," << CurrentTransportationCost - ImprovedTransportationCost << endl;
                                        ChangeInTransportationCost += ImprovedTransportationCost - CurrentTransportationCost;
                                        // cout << ChangeInTransportationCost << endl;
                                        double NewTransportationCost = IRPSolution.TotalTransportationCost + ChangeInTransportationCost;
                                        double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;

                                        // cout << "Route resulting in cheapest insertion:" << endl;
                                        // for (int y = 0; y < TempRoute[pick_day][pick_vehicle].size(); y++)
                                        //{
                                        //     cout << TempRoute[pick_day][pick_vehicle][y] << ",";
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
                                        // if (temp_LR_obvj < LR_objv)
                                        // {
                                        //     whether_improved_or_not = 1;
                                        //     ImpRoute = TempRoute;
                                        //     ImpInventoryLevel = TempInventoryLevel;
                                        //     ImpDeliveryQuantity = TempDeliveryQuantity;
                                        //     ImpUnallocatedCustomers = TempUnallocatedCustomers;
                                        //     ImpVehicleAllocation = TempVehicleAllocation;
                                        //     ImpVehicleLoad = TempVehicleLoad;
                                        //     ImpVisitOrder = TempVisitOrder;
                                        //     if (NewStockOut > 0)
                                        //     {
                                        //         ImpStockOut = NewStockOut;
                                        //     }
                                        //     else
                                        //     {
                                        //         ImpStockOut = 0;
                                        //         // cout << "!Improving feasible solution found" << endl;
                                        //     }

                                        //     // cout << "!Improving solution found" << endl;
                                        // }

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

        // Update the visit order
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

        cout << "Solution before rebalancing" << endl;
        IRPSolution.print_solution(IRPLR);

        // IRPSolution.GetLogisticRatio(IRPLR);
        // cout << IRPSolution.ViolationStockOut << "," << violation << endl;
        // assert(fabs(IRPSolution.ViolationStockOut - violation) < 0.001);
    }

    return whether_improved_or_not;
}