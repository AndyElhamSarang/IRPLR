#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR, preprocessing &memory,
                                               vector<vector<vector<int>>> &Route,        // Stay fixed if no redundant visit
                                               vector<vector<int>> &UnallocatedCustomers, // Stay fixed if no redundant visit
                                               vector<vector<double>> &VehicleLoad,
                                               vector<vector<double>> &DeliveryQuantity,
                                               vector<vector<double>> &InventoryLevel,
                                               vector<vector<int>> &VehicleAllocation, // Stay fixed if no redundant visit
                                               vector<vector<int>> &VisitOrder,        // Stay fixed if no redundant visit
                                               int &CountingInfeasibleCase,
                                               int &FeasibleRebalanceOrNot)
{
    FeasibleRebalanceOrNot = 0;
    // cout << "Balancing quantity operator" << endl;
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
            ///////////////////////////////////////////////////////////////
            //                                                           //
            //                For this time horizon,                     //
            //            The following procedure makes                  //
            //                the minimum delivery                       //
            //     so that no stock out happens until the next visit.    //
            //             Stop the rebalance operator                   //
            //    if vehicle capacity is already full at this stage.     //
            //                                                           //
            ///////////////////////////////////////////////////////////////
            for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
            {

                if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
                {
                    // assert(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]] > 0.00001);
                    double MinimumQuantityToSurvive = 0;
                    for (int time_i = time; time_i < NextVisitTime[customer][time]; time_i++)
                    {
                        // cout << MinimumQuantityToSurvive << "," << InventoryLevel[customer][time_i] << endl;
                        //  If inventory level is negative, need to deliver enough to make it non-negative
                        //  Pick the most negative inventory level to decide the minimum delivery quantity
                        if (MinimumQuantityToSurvive > InventoryLevel[customer][time_i])
                        {
                            MinimumQuantityToSurvive = InventoryLevel[customer][time_i];
                        }
                    }
                    MinimumQuantityToSurvive = -MinimumQuantityToSurvive;
                    cout<<"MinimumQuantityToSurvive:"<<MinimumQuantityToSurvive<<endl;
                    MinimumQuantityToSurvive = min(MinimumQuantityToSurvive, IRPLR.Retailers[customer].InventoryMax);
                    if (time == 0)
                    {
                        MinimumQuantityToSurvive = min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin);
                        cout<<"fabs(MinimumQuantityToSurvive):"<<fabs(MinimumQuantityToSurvive)<<", IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin:"<<IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin<<", min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin): "<<min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin)<<endl;
                    }
                    else
                    {
                        MinimumQuantityToSurvive = min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - InventoryLevel[customer][time - 1]);
                        cout<<"fabs(MinimumQuantityToSurvive):"<<fabs(MinimumQuantityToSurvive)<<", IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin:"<<IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin<<", min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - InventoryLevel[customer][time - 1]): "<<min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - InventoryLevel[customer][time - 1])<<endl;
                    }
                    assert(MinimumQuantityToSurvive >= 0);
                    DeliveryQuantity[customer][time] = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]],
                                                           MinimumQuantityToSurvive);
                    cout << "IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]]: " << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]] << ", MinimumQuantityToSurvive: " << MinimumQuantityToSurvive << endl;
                    assert(DeliveryQuantity[customer][time] <= IRPLR.Retailers[customer].InventoryMax);
                    assert(DeliveryQuantity[customer][time] >= 0);
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

            cout << "After deliver minimum to survive on time " << time << endl;
            PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
        }

        ///////////////////////////////////////////////////////////////
        //                                                           //
        //                         Repair stage                      //
        //                                                           //
        ///////////////////////////////////////////////////////////////

        // bool StockoutResolved = false;
        vector<vector<int>> CustomerStockoutTime;
        vector<double> CustomerStockout;

        for (int customer = 0; customer < InventoryLevel.size(); customer++)
        {
            double StockoutAmount = 0;
            int StockOutTime = IRPLR.TimeHorizon + 1;
            for (int time = 0; time < InventoryLevel[customer].size(); time++)
            {
                if (InventoryLevel[customer][time] < -0.00001)
                {
                    if (StockoutAmount < -InventoryLevel[customer][time]) // Getting the stockout time and the largest stockout amount for this customer
                    {
                        StockoutAmount = -InventoryLevel[customer][time];
                        StockOutTime = time;
                    }
                    // cout << "Customer " << customer << " has negative inventory level " << InventoryLevel[customer][time] << " at time " << time << endl;
                    // throw InventoryLevel[customer][time];
                }
            }
            if (StockoutAmount > 0.00001)
            {
                assert(StockoutAmount > 0);
                assert(StockOutTime < IRPLR.TimeHorizon);
                CustomerStockoutTime.push_back({customer, StockOutTime});
                CustomerStockout.push_back(StockoutAmount);
            }
        }
        for (int i = 0; i < CustomerStockoutTime.size(); i++)
        {

            cout << "Customer " << CustomerStockoutTime[i][0] << " has stockout " << CustomerStockout[i] << " at time " << CustomerStockoutTime[i][1] << endl;
        }
        // What I know: Customer, stockout amount, stockout time
        // Customer visited time and vehicle
        // Find customers in the same vehicle
        // Reduce their delivery quantity by increase the delivery quantity from the days before,
        //
        if (CustomerStockoutTime.size() != 0)
        {
            // If a customer has stockout, find where this customer is visited where a slack exists, i.e., delivery quantity is less than maximum inventory.

            vector<vector<vector<int>>> StockoutCustomerSchedule;
            // index 0, customer id
            // index 1, visit date
            // index 2, visit vehicle
            vector<vector<double>> QuantityCanAdjust;
            for (int StockoutCustomer = 0; StockoutCustomer < CustomerStockoutTime.size(); StockoutCustomer++)
            {
                vector<vector<int>> TempStockoutCustomerSchedule;
                vector<double> TempQuantityCanAdjust;
                // cout<<"Cumstomer "<<CustomerStockoutTime[StockoutCustomer][0]<< " is visited at: ";
                for (int j = 0; j < VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]].size(); j++)
                {
                    if (j <= CustomerStockoutTime[StockoutCustomer][1]) // Intersted in visits before or at the stockout time
                    {
                        if (VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]][j] < IRPLR.NumberOfVehicles)
                        {
                            if (DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] < IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax)
                            {
                                // cout<<"t"<<j<<"(Vehicle "<<VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]][j]<<"), ";
                                TempStockoutCustomerSchedule.push_back({CustomerStockoutTime[StockoutCustomer][0], j, VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]][j]});
                                TempQuantityCanAdjust.push_back(min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j], 0.0)));
                            }
                        }
                    }
                }
                StockoutCustomerSchedule.push_back(TempStockoutCustomerSchedule);
                QuantityCanAdjust.push_back(TempQuantityCanAdjust);
                // cout<<endl;
            }
            for (int i = 0; i < StockoutCustomerSchedule.size(); i++) // For each stockout customer,
            {
                for (int j = 0; j < StockoutCustomerSchedule[i].size(); j++) // For each visit of this stockout customer,
                {
                    cout << "Customer " << StockoutCustomerSchedule[i][j][0] << " is visited at time " << StockoutCustomerSchedule[i][j][1] << " via vehicle " << StockoutCustomerSchedule[i][j][2] << " with adjust quantity " << QuantityCanAdjust[i][j] << endl;
                }
                cout << "------------------------------" << endl;
            }
            vector<vector<vector<vector<int>>>> CustomerCanAdjust;
            for (int StockoutCustomer = 0; StockoutCustomer < StockoutCustomerSchedule.size(); StockoutCustomer++)
            {
                vector<vector<vector<int>>> TempCustomerCanAdjust;
                for (int StockoutCustomerVisit = 0; StockoutCustomerVisit < StockoutCustomerSchedule[StockoutCustomer].size(); StockoutCustomerVisit++)
                {
                    // cout << "Addressing stockout for customer " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0] << " at time " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] << " via vehicle " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][2] << endl;
                    vector<vector<int>> TempTempCustomerCanAdjust;
                    for (int i = 0; i < VehicleAllocation.size(); i++)
                    {
                        bool ThisCustomerIsVisitedAtTheSameTimeWithStockoutCustomer = true;
                        for (int j = 0; j < CustomerStockoutTime.size(); j++)
                        {
                            if (i != StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0])
                            {
                                if (i == CustomerStockoutTime[j][0])
                                {
                                    ThisCustomerIsVisitedAtTheSameTimeWithStockoutCustomer = false; // Excluding stockout customers
                                }
                            }
                        }
                        if (ThisCustomerIsVisitedAtTheSameTimeWithStockoutCustomer == true)
                        {
                            for (int j = 0; j < VehicleAllocation[i].size(); j++)
                            {
                                if (j == StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] && VehicleAllocation[i][j] == StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][2])
                                {
                                    // cout << "Vehicle " << VehicleAllocation[i][j] << " is also visiting customer " << i << " at day " << j << " while visiting the stockout customer." << endl;

                                    TempTempCustomerCanAdjust.push_back({i, j});
                                }
                            }
                        }
                    }
                    TempCustomerCanAdjust.push_back(TempTempCustomerCanAdjust);
                }
                CustomerCanAdjust.push_back(TempCustomerCanAdjust);
            }
            for (int i = 0; i < CustomerCanAdjust.size(); i++)
            {
                for (int j = 0; j < CustomerCanAdjust[i].size(); j++)
                {
                    cout << "For stockout customer " << StockoutCustomerSchedule[i][j][0] << ", at time " << StockoutCustomerSchedule[i][j][1] << ", via vehicle " << StockoutCustomerSchedule[i][j][2] << ", the following customers can adjust their delivery quantity: " << endl;
                    for (int k = 0; k < CustomerCanAdjust[i][j].size(); k++)
                    {
                        cout << "Customer " << CustomerCanAdjust[i][j][k][0] << ", at day " << CustomerCanAdjust[i][j][k][1] << endl;
                    }
                    cout << "------------------------------" << endl;
                }
            }
            for (int StockoutCustomer = 0; StockoutCustomer < CustomerCanAdjust.size(); StockoutCustomer++)
            {
                try
                {
                    for (int StockoutCustomerVisit = 0; StockoutCustomerVisit < CustomerCanAdjust[StockoutCustomer].size(); StockoutCustomerVisit++)
                    {
                        for (int CanAdjustCustomer = 0; CanAdjustCustomer < CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit].size(); CanAdjustCustomer++)
                        {

                            int CustomerIDCanAdjust = CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit][CanAdjustCustomer][0];
                            int TimeOfThisVisit = CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit][CanAdjustCustomer][1];
                            int VehicleOfThisVisit = VehicleAllocation[CustomerIDCanAdjust][TimeOfThisVisit];
                            bool SlackAvailable = false;

                            for (int i = 0; i < TimeOfThisVisit; i++) // check if there is slack in any of the previous visits
                            {
                                if (VehicleAllocation[CustomerIDCanAdjust][i] < IRPLR.NumberOfVehicles)
                                {
cout<<"IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - DeliveryQuantity[CustomerIDCanAdjust][i]: "<<IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - DeliveryQuantity[CustomerIDCanAdjust][i]<<", IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]]:"<<IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]] << endl;
                                    double AvailableSlack = min(IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - DeliveryQuantity[CustomerIDCanAdjust][i], IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]]);
                                    cout<< "available slack for customer " << CustomerIDCanAdjust << " at time " << i << " is " << AvailableSlack << " delivery quantity:" << DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] << endl;
                                    AvailableSlack = min(AvailableSlack, DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit]); //Cannot adjust more than the delivery quantity at TimeOfThisVisit
                                    if (AvailableSlack > 0.00001)
                                    {
                                        double AdjustQuantity = min(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit], AvailableSlack);
                                        DeliveryQuantity[CustomerIDCanAdjust][i] += AdjustQuantity;
                                        VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]] += AdjustQuantity;
                                        DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] -= AdjustQuantity;
                                        VehicleLoad[TimeOfThisVisit][VehicleAllocation[CustomerIDCanAdjust][TimeOfThisVisit]] -= AdjustQuantity;

                                        cout << "Customer " << CustomerIDCanAdjust << " has a visit at time " << i << " via vehicle " << VehicleAllocation[CustomerIDCanAdjust][i] << " with adjustable quantity " << AdjustQuantity << endl;
                                        if (i == 0)
                                        {
                                            double tempInventory = IRPLR.Retailers[CustomerIDCanAdjust].InventoryBegin;
                                            for (int y = 0; y < InventoryLevel[CustomerIDCanAdjust].size(); y++)
                                            {

                                                tempInventory = tempInventory - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                                                InventoryLevel[CustomerIDCanAdjust][y] = tempInventory;
                                            }
                                        }
                                        else
                                        {

                                            double tempInventory = InventoryLevel[CustomerIDCanAdjust][i - 1];
                                            for (int y = i; y < InventoryLevel[CustomerIDCanAdjust].size(); y++)
                                            {
                                                tempInventory = tempInventory - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                                                InventoryLevel[CustomerIDCanAdjust][y] = tempInventory;
                                            }
                                        }
                                        DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]] += AdjustQuantity;
                                        VehicleLoad[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][2]] += AdjustQuantity;

                                        if (i == 0)
                                        {
                                            
                                            double tempInventory = IRPLR.Retailers[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].InventoryBegin;
                                            for (int y = 0; y < InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].size(); y++)
                                            {

                                                tempInventory = tempInventory - IRPLR.Retailers[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].Demand + DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][y];
                                                InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][y] = tempInventory;
                                            }
                                        }
                                        else
                                        {
                                            double tempInventory = InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][i - 1];
                                            for (int y = i; y < InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].size(); y++)
                                            {
                                                tempInventory = tempInventory - IRPLR.Retailers[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].Demand + DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][y];
                                                InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][y] = tempInventory;
                                            }
                                        }
                                        assert(DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit] <= IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax);
                                        QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] = min(fabs(CustomerStockout[StockoutCustomer]) - AdjustQuantity, max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit], 0.0));
                                        cout<<"fabs(CustomerStockout[StockoutCustomer]):"<<fabs(CustomerStockout[StockoutCustomer])<<", AdjustQuantity: "<<AdjustQuantity<<", IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax:"<<IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax<<", DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit]: "<<DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit]<<endl;
                                        PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
                                        cout << "Check if stockout is resolved for customer " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0] << " QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] " << QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] << endl;
                                        bool StockoutResolved = true;
                                        for (int y = 0; y < InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].size(); y++)
                                        {

                                            if (InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][y] < -0.00001)
                                            {
                                                StockoutResolved = false;
                                            }
                                        }
                                        if (StockoutResolved == true)
                                        {
                                            // assert(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] == 0);
                                            throw StockoutResolved;
                                        }
                                    }
                                }
                            }
                            // double AvailableSlack = IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit];
                            // double QuantityToAdjust = min(AvailableSlack, fabs(CustomerStockout[StockoutCustomer]));
                            // DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] += QuantityToAdjust;
                            // VehicleLoad[TimeOfThisVisit][VehicleOfThisVisit] += QuantityToAdjust;
                            // if (TimeOfThisVisit == 0)
                            // {
                            //     double tempInventory = IRPLR.Retailers[CustomerIDCanAdjust].InventoryBegin;
                            //     for (int y = 0; y < InventoryLevel[CustomerIDCanAdjust].size(); y++)
                            //     {

                            //         tempInventory = tempInventory - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                            //         InventoryLevel[CustomerIDCanAdjust][y] = tempInventory;
                            //     }
                            // }
                            // else
                            // {

                            //     double tempInventory = InventoryLevel[CustomerIDCanAdjust][TimeOfThisVisit - 1];
                            //     for (int y = TimeOfThisVisit; y < InventoryLevel[CustomerIDCanAdjust].size(); y++)
                            //     {
                            //         tempInventory = tempInventory - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                            //         InventoryLevel[CustomerIDCanAdjust][y] = tempInventory;
                            //     }
                            // }
                            // CustomerStockout[StockoutCustomer] += QuantityToAdjust;
                            // if (fabs(CustomerStockout[StockoutCustomer]) < 0.00001)
                            // {
                            //     StockoutResolved = true;
                            //     break;
                            // }
                        }
                    }
                }
                catch (bool StockoutResolved)
                {
                    cout << "Stockout is resolved for this customer." << endl;
                }
                // if (StockoutResolved == true)
                // {
                //     break;
                // }
            }
        }

        for (int customer = 0; customer < InventoryLevel.size(); customer++)
        {
            for (int time = 0; time < InventoryLevel[customer].size(); time++)
            {
                if (InventoryLevel[customer][time] < -0.00001)
                {
                    cout << "Customer " << customer << " has negative inventory level " << InventoryLevel[customer][time] << " at time " << time << endl;
                    throw InventoryLevel[customer][time];
                }
            }
        }
        ///////////////////////////////////////////////////////////////
        //                                                           //
        //                      Rebalance Stage 2                   //
        //                                                           //
        ///////////////////////////////////////////////////////////////

        for (int time = 0; time < IRPLR.TimeHorizon; time++)
        {

            // ///////////////////////////////////////////////////////////////////////////
            // //                                                                       //
            // //     At this stage, the schedule of this time is already feasible      //
            // //           the following code removes unnecessary visits               //
            // //                  to avoid high travel costs.                          //
            // //                                                                       //
            // ///////////////////////////////////////////////////////////////////////////

            // // Search for an existing visit that delivers nothing, find the visit in the route and delete it
            // // Otherwise if an existing visit delivers, computes the total delivery quantity by incrementing the delivery quantity of each visit.

            // for (int i = 0; i < DeliveryQuantity.size(); i++) // Index i for retailer
            // {

            //     if (DeliveryQuantity[i][time] == 0 && VehicleAllocation[i][time] < IRPLR.NumberOfVehicles)
            //     {
            //         UnallocatedCustomers[time].push_back(Route[time][VehicleAllocation[i][time]][VisitOrder[i][time]]);
            //         Route[time][VehicleAllocation[i][time]].erase(Route[time][VehicleAllocation[i][time]].begin() + VisitOrder[i][time], Route[time][VehicleAllocation[i][time]].begin() + VisitOrder[i][time] + 1);

            //         for (int k = 0; k < VisitOrder.size(); k++)
            //         {
            //             if (VehicleAllocation[k][time] == VehicleAllocation[i][time]) // Find visits in the same vehicle as Retailer i on day j
            //             {
            //                 if (VisitOrder[k][time] > VisitOrder[i][time]) // If this visit is after the visit of Retailer i
            //                 {
            //                     VisitOrder[k][time] = VisitOrder[k][time] - 1;
            //                 }
            //             }
            //         }
            //         VisitOrder[i][time] = IRPLR.Retailers.size() + 1;
            //         VehicleAllocation[i][time] = IRPLR.NumberOfVehicles + 1;
            //     }
            //     else
            //     {
            //         if (VehicleAllocation[i][time] == IRPLR.NumberOfVehicles + 1)
            //         {
            //             assert(DeliveryQuantity[i][time] == 0);
            //         }
            //     }
            // }

            // cout << "After deliver minimum to survive on time "  <<time<< endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
            // //////////////////////////////////////////////////////////////
            // //                                                          //
            // //     For the remaining capacity of vehicle,               //
            // //            deliver as much as possible                   //
            // //         with a random sequence of customer,              //
            // //       the probability are choosen adaptively.            //
            // //                                                          //
            // //////////////////////////////////////////////////////////////

            // vector<vector<int>> CustomerWeight; // index 0 customer id // index 1 Weight
            // // cout << "CustomerWeight for time period:" <<time<< endl;
            // for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
            // {
            //     if (VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
            //     {
            //         vector<int> TempCustomerWeight;
            //         TempCustomerWeight.push_back(customer);
            //         double TempCusomterWeight = 0;
            //         if (time == 0)
            //         {
            //             TempCusomterWeight = (IRPLR.Retailers[customer].InventoryBegin) / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * (MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] + 1));

            //             // cout << "customer " << customer << ": " << TempCusomterWeight
            //             //      << " ; " << 1 / TempCusomterWeight << " ; " << IRPLR.Retailers[customer].InventoryBegin + 1
            //             //      << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
            //             //      << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
            //         }
            //         else
            //         {

            //             TempCusomterWeight = (InventoryLevel[customer][time - 1] + 1) / (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * (MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] + 1));

            //             // cout << "customer " << customer << ": " << TempCusomterWeight
            //             //      << " ; " << 1 / TempCusomterWeight << " ; " << InventoryLevel[customer][time - 1] + 1
            //             //      << " ; " << (IRPLR.Retailers[customer].Demand * (NextVisitTime[customer][time] - time) * MaxNumberOfConseuctiveDaysACustomerNotVisited[customer])
            //             //      << " ; " << IRPLR.Retailers[customer].Demand << " ; " << NextVisitTime[customer][time] - time << " ; " << MaxNumberOfConseuctiveDaysACustomerNotVisited[customer] << endl;
            //             if (0 - InventoryLevel[customer][time - 1] > 0.00001)
            //             {
            //                 throw InventoryLevel[customer][time - 1];
            //             }
            //         }

            //         TempCustomerWeight.push_back((1 / TempCusomterWeight) * 100);
            //         CustomerWeight.push_back(TempCustomerWeight);
            //     }
            // }

            // // for (int i = 0; i < CustomerWeight.size(); i++)
            // // {
            // //     cout << "customer " << CustomerWeight[i][0] << ": " << CustomerWeight[i][1] << endl;
            // // }
            // double CumulativeWeight = 0;
            // while (CustomerWeight.size() != 0)
            // {
            //     vector<vector<int>> CumulativeCustomerWeight(CustomerWeight);

            //     CumulativeWeight = 0;
            //     for (int i = 0; i < CumulativeCustomerWeight.size(); i++)
            //     {
            //         CumulativeWeight += CumulativeCustomerWeight[i][1];
            //         CumulativeCustomerWeight[i][1] = CumulativeWeight;
            //     }
            //     // cout << "Cumulative weight:" << CumulativeWeight << endl;
            //     // for (int i = 0; i < CumulativeCustomerWeight.size(); i++)
            //     // {
            //     //     cout << "customer " << CumulativeCustomerWeight[i][0] << ": " << CumulativeCustomerWeight[i][1] << endl;
            //     // }

            //     int SelectedWeight = RandomnessInBalance.random_number_generator(0, CumulativeWeight, generator);

            //     int RandomCustomer = CumulativeCustomerWeight.size() + 1;

            //     if (SelectedWeight <= CumulativeCustomerWeight[0][1])
            //     {
            //         RandomCustomer = 0;
            //     }
            //     else
            //     {
            //         for (int i = 0; i < CumulativeCustomerWeight.size() - 1; i++)
            //         {
            //             if (CumulativeCustomerWeight[i][1] < SelectedWeight && SelectedWeight <= CumulativeCustomerWeight[i + 1][1])
            //             {
            //                 RandomCustomer = i + 1;
            //             }
            //         }
            //     }
            //     // cout << "SelectedWeight:" << SelectedWeight << endl;
            //     assert(RandomCustomer < CumulativeCustomerWeight.size());
            //     // cout << "Customer " << CumulativeCustomerWeight[RandomCustomer][0] << " with row " << RandomCustomer << " is selected" << endl;

            //     double DeliverMore = 0;
            //     if (time == 0)
            //     {
            //         assert(IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] <= IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax + 0.00001);
            //         DeliverMore = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
            //                           IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin - DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time]);
            //         // cout << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin
            //         //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax
            //         //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin << endl;
            //         // cout << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]]
            //         //      << "," << IRPLR.Vehicle.capacity
            //         //      << "," << VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] << endl;
            //     }
            //     else
            //     {
            //         assert(InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] <= IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax + 0.00001);

            //         DeliverMore = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
            //                           IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] - DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time]);
            //         // cout << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1]
            //         //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax
            //         //      << "," << InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] << endl;
            //         // cout << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]]
            //         //      << "," << IRPLR.Vehicle.capacity
            //         //      << "," << VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] << endl;
            //     }

            //     assert(DeliverMore >= 0);
            //     DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] += DeliverMore;
            //     VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] += DeliverMore;

            //     assert(DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] >= 0);
            //     if (time == 0)
            //     {
            //         double tempInventory = IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin;
            //         for (int y = 0; y < InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]].size(); y++)
            //         {

            //             tempInventory = tempInventory - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].Demand + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][y];
            //             InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][y] = tempInventory;
            //         }
            //     }
            //     else
            //     {

            //         double tempInventory = InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1];
            //         for (int y = time; y < InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]].size(); y++)
            //         {
            //             tempInventory = tempInventory - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].Demand + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][y];
            //             InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][y] = tempInventory;
            //         }
            //     }

            //     CustomerWeight.erase(CustomerWeight.begin() + RandomCustomer, CustomerWeight.begin() + RandomCustomer + 1);
            //     CumulativeCustomerWeight.erase(CumulativeCustomerWeight.begin() + RandomCustomer, CumulativeCustomerWeight.begin() + RandomCustomer + 1);
            // }
            // cout << "After complete rebalancing, time " <<time<< endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);

            // assert(aborter == 1);
        }

        // Search for an existing visit that delivers nothing, find the visit in the route and delete it
        // Otherwise if an existing visit delivers, computes the total delivery quantity by incrementing the delivery quantity of each visit.
        double temp_total_delivery_quantity = 0;
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
        // cout << "Rebalance found feasible solution" << endl;
        // cout<<"Logistic ratio:"<<LogisticRatio<<", Total transportation cost:"<< temp_total_transportation_cost<< ", Total delivery quantity:"<<temp_total_delivery_quantity<<endl;
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
        // assert(fabs(TempTransportationCost - NewTransportationCost) < 0.00001);
        // cout << "Check Delivery Quantity" << endl;
        // cout << TempTotalDelivery << "," << NewTotalDelivery << endl;
        // assert(fabs(TempTotalDelivery - NewTotalDelivery) < 0.00001);
        // cout << "Check Logistic ratio" << endl;
        // cout << TempLogisticRatio << "," << NewLogisticRatio << endl;
        // assert(fabs(TempLogisticRatio - NewLogisticRatio) < 0.00001);
        // cout << "Check ViolationStockout" << endl;
        // assert(TempViolationStockOut < 0.00001);
        // cout << NewLogisticRatio << "," << CurrentLogisticRatio << endl;

        // assert(aborter == 1);
    }
    catch (double rebalacing_facing_stock_out)
    {
        CountingInfeasibleCase++;
        cout << "rebalacing_facing_stock_out, break" << endl;
    }
    // cout << "End balancing" << endl;
    // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation,VisitOrder);
    // cout << "Exit balancing" << endl;
    // assert(aborter == 1);
    return LogisticRatio;
}