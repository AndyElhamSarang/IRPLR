#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR, preprocessing &memory,
                                               vector<vector<vector<int>>> &Route,        // Stay fixed if no redundant visit
                                               vector<vector<int>> &UnallocatedCustomers, // Stay fixed if no redundant visit
                                               vector<vector<double>> &VehicleLoad,
                                               vector<vector<double>> &DeliveryQuantity,
                                               vector<vector<double>> &InventoryLevel,
                                               vector<vector<int>> &VehicleAllocation, // Stay fixed if no redundant visit
                                               vector<vector<int>> &VisitOrder, // Stay fixed if no redundant visit
                                               int & CountingInfeasibleCase,
                                               int & FeasibleRebalanceOrNot
)
{
    FeasibleRebalanceOrNot = 0;
    cout << "Balancing quantity operator" << endl;
    vector<int> MaxNumberOfConseuctiveDaysACustomerNotVisited;
    vector<vector<int>> NextVisitTime;
    vector<int> NumberOfVisits;
    // Reset the solution

    double LogisticRatio = 0;
    vector<int> AllCustomers;
    for (int i = 0; i < VehicleLoad.size(); i++)
    {
        for (int j = 0; j < VehicleLoad[i].size(); j++)
        {
            VehicleLoad[i][j] = 0;
        }
    }
    for (int i = 0; i < DeliveryQuantity.size(); i++)
    {

        vector<int> TempNextVisitTime;
        double tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < DeliveryQuantity[i].size(); j++)
        {

            TempNextVisitTime.push_back(IRPLR.TimeHorizon);
            DeliveryQuantity[i][j] = 0;
            InventoryLevel[i][j] = tempInventory - IRPLR.Retailers[i].Demand;
            ;
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand;
        }
        NextVisitTime.push_back(TempNextVisitTime);
        AllCustomers.push_back(i);
    }
    for (int customer = 0; customer < VehicleAllocation.size(); customer++)
    {
        int TempMaxNumberOfConseuctiveDaysACustomerNotVisited = 0;
        int TempNumberOfConseuctiveDaysACustomerNotVisited = 0;
        for (int time = 0; time < VehicleAllocation[customer].size(); time++)
        {
            if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
            {
                TempNumberOfConseuctiveDaysACustomerNotVisited = 0;
            }
            else
            {
                TempNumberOfConseuctiveDaysACustomerNotVisited++;
            }
            if (TempMaxNumberOfConseuctiveDaysACustomerNotVisited < TempNumberOfConseuctiveDaysACustomerNotVisited)
            {
                TempMaxNumberOfConseuctiveDaysACustomerNotVisited = TempNumberOfConseuctiveDaysACustomerNotVisited;
            }
        }
        MaxNumberOfConseuctiveDaysACustomerNotVisited.push_back(TempMaxNumberOfConseuctiveDaysACustomerNotVisited);
    }

    for (int i = 0; i < NextVisitTime.size(); i++) // For a customer
    {
        for (int j = 0; j < NextVisitTime[i].size() - 1; j++) // For a time period, not the last
        {
            for (int y = j + 1; y < VehicleAllocation[i].size(); y++) // Find when is the next visit period
            {
                if (VehicleAllocation[i][y] < IRPLR.NumberOfVehicles)
                {
                    NextVisitTime[i][j] = y;
                    break;
                }
            }
        }
    }

    int aborter = 0;
    // cout << "Next visit time" << endl;
    // for (int i = 0; i < NextVisitTime.size(); i++)
    // {
    //     for (int j = 0; j < NextVisitTime[i].size(); j++)
    //     {
    //         cout << NextVisitTime[i][j] << ",";
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    // cout << "Maximum number of consective days a customer is not visited" << endl;
    // for (int i = 0; i < MaxNumberOfConseuctiveDaysACustomerNotVisited.size(); i++)
    // {
    //     cout << "Customer " << i << ": " << MaxNumberOfConseuctiveDaysACustomerNotVisited[i] << endl;
    // }

    boost_random_mechanism RandomnessInBalance;
    assert(AllCustomers.size() != 0);
    try
    {
        for (int time = 0; time < IRPLR.TimeHorizon; time++)
        {
            // cout<<"Rebalanceing time "<< time<<endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
            for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
            {

                //////////////////////////////////////////////////////////////
                //                                                          //
                //     A procedure that make the minimum delivery           //
                //   so that no stock out happens until the next visit.     //
                //             Stop the rebalance operator                  //
                //    if vehicle capacity is already full at this stage.    //
                //                                                          //
                //////////////////////////////////////////////////////////////
                if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
                {
                    // assert(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]] > 0.001);
                    double MinimumQuantityToSurvive = 0;
                    for (int time_i = time; time_i < NextVisitTime[customer][time]; time_i++)
                    {
                        //cout << MinimumQuantityToSurvive << "," << InventoryLevel[customer][time_i] << endl;
                        if (MinimumQuantityToSurvive > InventoryLevel[customer][time_i])
                        {
                            MinimumQuantityToSurvive = InventoryLevel[customer][time_i];
                        }
                    }
                    MinimumQuantityToSurvive = min(fabs(MinimumQuantityToSurvive),
                                                   IRPLR.Retailers[customer].InventoryMax - InventoryLevel[customer][time]);

                    DeliveryQuantity[customer][time] = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]],
                                                           MinimumQuantityToSurvive);

                    VehicleLoad[time][VehicleAllocation[customer][time]] += DeliveryQuantity[customer][time];

                    if (time == 0)
                    {
                        double tempInventory = IRPLR.Retailers[customer].InventoryBegin;
                        for (int y = 0; y < InventoryLevel[customer].size(); y++)
                        {

                            tempInventory = tempInventory - IRPLR.Retailers[customer].Demand + DeliveryQuantity[customer][y];
                            InventoryLevel[customer][y] = tempInventory;
                        }
                    }
                    else
                    {

                        double tempInventory = InventoryLevel[customer][time - 1];
                        for (int y = time; y < InventoryLevel[customer].size(); y++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[customer].Demand + DeliveryQuantity[customer][y];
                            InventoryLevel[customer][y] = tempInventory;
                        }
                    }

                }
            }

            // cout << "After deliver minimum to survive on time "  <<time<< endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
            //////////////////////////////////////////////////////////////
            //                                                          //
            //     For the remaining capacity of vehicle,               //
            //            deliver as much as possible                   //
            //         with a random sequence of customer,              //
            //       the probability are choosen adaptively.            //
            //                                                          //
            //////////////////////////////////////////////////////////////

            vector<vector<int>> CustomerWeight; // index 0 customer id // index 1 Weight
            // cout << "CustomerWeight for time period:" <<time<< endl;
            for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
            {
                if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
                {
                    vector<int> TempCustomerWeight;
                    TempCustomerWeight.push_back(customer);
                    double TempCusomterWeight = 0;
                    if (time == 0)
                    {
                        TempCusomterWeight = (IRPLR.Retailers[customer].InventoryBegin) / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * (MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] + 1));

                        // cout << "customer " << customer << ": " << TempCusomterWeight
                        //      << " ; " << 1 / TempCusomterWeight << " ; " << IRPLR.Retailers[customer].InventoryBegin + 1
                        //      << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
                        //      << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
                    }
                    else
                    {
                       
                        TempCusomterWeight = (InventoryLevel[customer][time - 1] + 1) / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * (MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] + 1));

                        // cout << "customer " << customer << ": " << TempCusomterWeight
                        //      << " ; " << 1 / TempCusomterWeight << " ; " << InventoryLevel[customer][time - 1] + 1
                        //      << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
                        //      << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
                        if (0 - InventoryLevel[customer][time - 1] >0.001)
                        {
                            throw InventoryLevel[customer][time - 1];
                        }
                    }

                    TempCustomerWeight.push_back((1 / TempCusomterWeight) * 100);
                    CustomerWeight.push_back(TempCustomerWeight);
                }
            }

            // for (int i = 0; i < CustomerWeight.size(); i++)
            // {
            //     cout << "customer " << CustomerWeight[i][0] << ": " << CustomerWeight[i][1] << endl;
            // }
            double CumulativeWeight = 0;
            while (CustomerWeight.size() != 0)
            {
                vector<vector<int>> CumulativeCustomerWeight(CustomerWeight);

                CumulativeWeight = 0;
                for (int i = 0; i < CumulativeCustomerWeight.size(); i++)
                {
                    CumulativeWeight += CumulativeCustomerWeight[i][1];
                    CumulativeCustomerWeight[i][1] = CumulativeWeight;
                }
                // cout << "Cumulative weight:" << CumulativeWeight << endl;
                // for (int i = 0; i < CumulativeCustomerWeight.size(); i++)
                // {
                //     cout << "customer " << CumulativeCustomerWeight[i][0] << ": " << CumulativeCustomerWeight[i][1] << endl;
                // }

                int SelectedWeight = RandomnessInBalance.random_number_generator(0, CumulativeWeight, generator);

                int RandomCustomer = CumulativeCustomerWeight.size() + 1;

                if (SelectedWeight <= CumulativeCustomerWeight[0][1])
                {
                    RandomCustomer = 0;
                }
                else
                {
                    for (int i = 0; i < CumulativeCustomerWeight.size() - 1; i++)
                    {
                        if (CumulativeCustomerWeight[i][1] < SelectedWeight && SelectedWeight <= CumulativeCustomerWeight[i + 1][1])
                        {
                            RandomCustomer = i + 1;
                        }
                    }
                }
                // cout << "SelectedWeight:" << SelectedWeight << endl;
                assert(RandomCustomer < CumulativeCustomerWeight.size());
                // cout << "Customer " << CumulativeCustomerWeight[RandomCustomer][0] << " with row " << RandomCustomer << " is selected" << endl;

                double DeliverMore =
                    min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
                        IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time]);

                assert(DeliverMore >= 0);
                DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] += DeliverMore;
                VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] += DeliverMore;

                // cout << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time]
                //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax
                //      << "," << InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time] << endl;
                // cout << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]]
                //      << "," << IRPLR.Vehicle.capacity
                //      << "," << VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] << endl;

                assert(DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] >= 0);
                if (time == 0)
                {
                    double tempInventory = IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin;
                    for (int y = 0; y < InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]].size(); y++)
                    {

                        tempInventory = tempInventory - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].Demand + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][y];
                        InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][y] = tempInventory;
                    }
                }
                else
                {

                    double tempInventory = InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1];
                    for (int y = time; y < InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]].size(); y++)
                    {
                        tempInventory = tempInventory - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].Demand + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][y];
                        InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][y] = tempInventory;
                    }
                }

                CustomerWeight.erase(CustomerWeight.begin() + RandomCustomer, CustomerWeight.begin() + RandomCustomer + 1);
                CumulativeCustomerWeight.erase(CumulativeCustomerWeight.begin() + RandomCustomer, CumulativeCustomerWeight.begin() + RandomCustomer + 1);
            }
            // cout << "After complete rebalancing, time " <<time<< endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
           
            // assert(aborter == 1);
        }
        for(int customer=0; customer < InventoryLevel.size();customer++)
        {
            if (InventoryLevel[customer][InventoryLevel[customer].size()-1] < -0.001)
            {
                throw InventoryLevel[customer][InventoryLevel[customer].size()-1];
            }
        }

        // If solution made to this stage, it is feasible
        FeasibleRebalanceOrNot = 1;
        //Search for an existing visit that delivers nothing, find the the visit in the route and delete it
        //Otherwise if an existingi vist delivers, increment the delivery quantity.
        double temp_total_delivery_quantity=0;
        for (int i = 0; i < DeliveryQuantity.size(); i++) // Index i for retailer
        {
            for (int j = 0; j < DeliveryQuantity[i].size(); j++) // Index j for time period
            {
                if (DeliveryQuantity[i][j] == 0 && VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
                {
                    UnallocatedCustomers[j].push_back(Route[j][VehicleAllocation[i][j]][VisitOrder[i][j]]);
                    Route[j][VehicleAllocation[i][j]].erase(Route[j][VehicleAllocation[i][j]].begin() + VisitOrder[i][j], Route[j][VehicleAllocation[i][j]].begin() + VisitOrder[i][j] + 1);

                    // Here insert code to achieve fast evaluation on travel time

                    for (int k = 0; k < VisitOrder.size(); k++)
                    {
                        if (VehicleAllocation[k][j] == VehicleAllocation[i][j]) // Find visits in the same vehicle as Retailer i on day j
                        {
                            if (VisitOrder[k][j] > VisitOrder[i][j]) // If this visit is after the visit of Retailer i
                            {
                                VisitOrder[k][j] = VisitOrder[k][j] - 1;
                            }
                        }
                    }
                    VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
                    VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
                }
                else
                {
                    if (VehicleAllocation[i][j] == IRPLR.NumberOfVehicles + 1)
                    {
                        assert(DeliveryQuantity[i][j] == 0);
                    }
                    temp_total_delivery_quantity += DeliveryQuantity[i][j];
                }
            }
        } 
        double temp_total_transportation_cost = 0;
        for (int i = 0; i < Route.size(); i++)
        {
            double TransportationCostForTheDay = 0;
            for (int j = 0; j < Route[i].size(); j++)
            {

                if (Route[i][j].size() != 0)
                {
                    TransportationCostForTheDay += IRPLR.Distance[0][Route[i][j][0] + 1];

                    for (int k = 0; k < Route[i][j].size() - 1; k++)
                    {
                        TransportationCostForTheDay += IRPLR.Distance[Route[i][j][k] + 1][Route[i][j][k + 1] + 1];
                    }
                    TransportationCostForTheDay += IRPLR.Distance[Route[i][j][Route[i][j].size() - 1] + 1][0];
                }
            }
            // cout<<"TransportationCostForTheDay:"<<TransportationCostForTheDay<<endl;
            temp_total_transportation_cost += TransportationCostForTheDay;
        }

        LogisticRatio = temp_total_transportation_cost / temp_total_delivery_quantity;
        cout << "Rebalance found feasible solution" << endl;
        cout<<"Logistic ratio:"<<LogisticRatio<<", Total transportation cost:"<< temp_total_transportation_cost<< ", Total delivery quantity:"<<temp_total_delivery_quantity<<endl;
        // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);

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
        // assert(TempViolationStockOut < 0.001);
        // cout << NewLogisticRatio << "," << CurrentLogisticRatio << endl;

        // assert(aborter == 1);

    }
    catch (double rebalacing_facing_stock_out)
    {
        CountingInfeasibleCase++;
        cout<<"rebalacing_facing_stock_out, break"<<endl;
    }
    // cout << "End balancing" << endl;
    // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
    // cout << "Exit balancing" << endl;
    //assert(aborter == 1);
    return LogisticRatio;
}