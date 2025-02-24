#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR, preprocessing &memory,
                                               vector<vector<vector<int>>> &Route,        // Stay fixed
                                               vector<vector<int>> &UnallocatedCustomers, // Stay fixed
                                               vector<vector<double>> &VehicleLoad,
                                               vector<vector<double>> &DeliveryQuantity,
                                               vector<vector<double>> &InventoryLevel,
                                               vector<vector<int>> &VehicleAllocation // Stay fixed
)
{
    cout << "Balancing quantity operator" << endl;
    vector<int> MaxNumberOfConseuctiveDaysACustomerNotVisited;
    vector<vector<int>> NextVisitTime;
    vector<int> NumberOfVisits;
    // Reset the solution

    double objv = 0;
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
    cout << "Next visit time" << endl;
    for (int i = 0; i < NextVisitTime.size(); i++)
    {
        for (int j = 0; j < NextVisitTime[i].size(); j++)
        {
            cout << NextVisitTime[i][j] << ",";
        }
        cout << endl;
    }
    cout << endl;
    cout << "Maximum number of consective days a customer is not visited" << endl;
    for (int i = 0; i < MaxNumberOfConseuctiveDaysACustomerNotVisited.size(); i++)
    {
        cout << "Customer " << i << ": " << MaxNumberOfConseuctiveDaysACustomerNotVisited[i] << endl;
    }

    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    boost_random_mechanism RandomnessInBalance;
    assert(AllCustomers.size() != 0);

    for (int time = 0; time < IRPLR.TimeHorizon; time++)
    {
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
                    cout << MinimumQuantityToSurvive << "," << InventoryLevel[customer][time_i] << endl;
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

                cout << "After deliver minimum to survive" << endl;
            }
        }

        PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
        //////////////////////////////////////////////////////////////
        //                                                          //
        //     For the remaining capacity of vehicle,               //
        //            deliver as much as possible                   //
        //         with a random sequence of customer,              //
        //       the probability are choosen adaptively.            //
        //                                                          //
        //////////////////////////////////////////////////////////////

        vector<vector<int>> CustomerWeight; // index 0 customer id // index 1 Weight
        cout << "CustomerWeight" << endl;
        for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
        {
            if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
            {
                vector<int> TempCustomerWeight;
                TempCustomerWeight.push_back(customer);
                double TempCusomterWeight = 0;
                if (time == 0)
                {
                    TempCusomterWeight = IRPLR.Retailers[customer].InventoryBegin / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer]);

                    cout << "customer " << customer << ": " << TempCusomterWeight
                         << " ; " << 1 / TempCusomterWeight << " ; " << IRPLR.Retailers[customer].InventoryBegin
                         << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
                         << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
                }
                else
                {
                    TempCusomterWeight = InventoryLevel[customer][time - 1] / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer]);

                    cout << "customer " << customer << ": " << TempCusomterWeight
                         << " ; " << 1 / TempCusomterWeight << " ; " << InventoryLevel[customer][time - 1]
                         << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
                         << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
                }

                TempCustomerWeight.push_back((1 / TempCusomterWeight) * 100);
                CustomerWeight.push_back(TempCustomerWeight);
            }
        }

        // Need remove customers who are not visited
        for(int i=0;i<CustomerWeight.size();i++)
        {
            cout << "customer " << CustomerWeight[i][0] << ": " << CustomerWeight[i][1] << endl;
        }
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
            cout << "Cumulative weight:" <<CumulativeWeight<< endl;
            for (int i = 0; i < CumulativeCustomerWeight.size(); i++)
            {
                cout << "customer " << CumulativeCustomerWeight[i][0] << ": " << CumulativeCustomerWeight[i][1] << endl;
            }

             int SelectedWeight= RandomnessInBalance.random_number_generator(0, CumulativeWeight, generator);

             int RandomCustomer = CumulativeCustomerWeight.size()+1;

             if ( SelectedWeight  < CumulativeCustomerWeight[0][1] )
             {
                RandomCustomer = 0;
             }
             else
             {
                 for (int i = 0; i < CumulativeCustomerWeight.size() - 1; i++)
                 {
                     if (CumulativeCustomerWeight[i][1] < SelectedWeight && SelectedWeight < CumulativeCustomerWeight[i + 1][1])
                     {
                         RandomCustomer = i + 1;
                     }
                 }
             }

            assert(RandomCustomer < CumulativeCustomerWeight.size());
            cout << "Customer " << CumulativeCustomerWeight[RandomCustomer][0] << " with row "<< RandomCustomer<<" is selected" << endl;

            DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] +=
                min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
                    IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time]);
            VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] += DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time];

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

        //assert(aborter == 1);
    }

    cout << "End balancing" << endl;
    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    assert(aborter == 1);
   
    cout << "Exit balancing" << endl;
    assert(aborter == 1);
    return objv;
}