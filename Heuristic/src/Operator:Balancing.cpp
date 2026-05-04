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
                    for (int time_i = time; time_i < IRPLR.TimeHorizon; time_i++)
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
                    // cout << "Largest stockout:" << MinimumQuantityToSurvive << endl;
                    if (time == 0)
                    {
                        MinimumQuantityToSurvive = min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin);
                        // cout << "fabs(MinimumQuantityToSurvive):" << fabs(MinimumQuantityToSurvive) << ", IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin:" << IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin << ", min(Largest stockout, IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin): " << min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - IRPLR.Retailers[customer].InventoryBegin) << endl;
                    }
                    else
                    {
                        MinimumQuantityToSurvive = min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - max(0.0, InventoryLevel[customer][time - 1]));
                        // cout << "fabs(MinimumQuantityToSurvive):" << fabs(MinimumQuantityToSurvive) << ",max(0.0, InventoryLevel[customer][time - 1]):" << max(0.0, InventoryLevel[customer][time - 1]) << ", min(Largest stockout, IRPLR.Retailers[customer].InventoryMax - InventoryLevel[customer][time - 1]): " << min(fabs(MinimumQuantityToSurvive), IRPLR.Retailers[customer].InventoryMax - max(0.0, InventoryLevel[customer][time - 1])) << endl;
                    }
                    assert(MinimumQuantityToSurvive >= 0);
                    DeliveryQuantity[customer][time] = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]],
                                                           MinimumQuantityToSurvive);
                    // cout << "IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]]: " << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[customer][time]] << ", MinimumQuantityToSurvive: " << MinimumQuantityToSurvive << endl;
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

            // cout << "After deliver minimum to survive on time " << time << endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
        }

        ///////////////////////////////////////////////////////////////
        //                                                           //
        //                         Repair stage                      //
        //                                                           //
        ///////////////////////////////////////////////////////////////

        bool StockoutResolved = false;
        bool SlackIsZero = false;
        while (StockoutResolved == false && SlackIsZero == false)
        {

            ///////////////////////////////////////////////////////////////
            //                                                           //
            //                Find if stockout exists                    //
            //         Record the time and amount of first stockout      //
            //                                                           //
            ///////////////////////////////////////////////////////////////
            vector<vector<int>> CustomerStockoutTime;
            // index 0, customer id; index 1, stockout time
            vector<double> CustomerStockout;
            bool StockoutExists = false;
            for (int customer = 0; customer < InventoryLevel.size(); customer++) // For each customer
            {
                for (int time = 0; time < InventoryLevel[customer].size(); time++) // For each time
                {
                    if (InventoryLevel[customer][time] < -0.00001)
                    {
                        if (StockoutExists == false)
                        {
                            CustomerStockoutTime.push_back({customer, time});
                            // cout << "Customer " << customer << " has stockout " << -InventoryLevel[customer][time] << " at time " << time << endl;
                            double StockoutAmount = -InventoryLevel[customer][time];
                            CustomerStockout.push_back(StockoutAmount);
                            StockoutExists = true;
                        }
                        // cout << "Customer " << customer << " has negative inventory level " << InventoryLevel[customer][time] << " at time " << time << endl;
                        // throw InventoryLevel[customer][time];
                    }
                }
            }
            for (int i = 0; i < CustomerStockoutTime.size(); i++)
            {

                // cout << "Customer " << CustomerStockoutTime[i][0] << " has stockout " << CustomerStockout[i] << " at time " << CustomerStockoutTime[i][1] << endl;
            }
            // What I know: Customer, stockout amount, stockout time
            // Customer visited time and vehicle
            // Find customers in the same vehicle
            // Reduce their delivery quantity by increase the delivery quantity from the days before,
            //

            if (CustomerStockoutTime.size() == 0)
            {
                StockoutResolved = true;
            }
            else if (CustomerStockoutTime.size() != 0)
            {
                // If a customer has stockout, find where this customer is visited, i.e., delivery quantity is less than maximum inventory.
                assert(CustomerStockoutTime.size() == 1);
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
                                if (j == 0)
                                {
                                    if (DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] < IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin)
                                    {
                                        TempStockoutCustomerSchedule.push_back({CustomerStockoutTime[StockoutCustomer][0], j, VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]][j]});
                                        TempQuantityCanAdjust.push_back(min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin, 0.0)));
                                        // cout << "min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin, 0.0):" << min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin, 0.0)) << endl;
                                    }
                                }
                                else
                                {
                                    if (DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] < IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - InventoryLevel[CustomerStockoutTime[StockoutCustomer][0]][j - 1])
                                    {
                                        TempStockoutCustomerSchedule.push_back({CustomerStockoutTime[StockoutCustomer][0], j, VehicleAllocation[CustomerStockoutTime[StockoutCustomer][0]][j]});
                                        TempQuantityCanAdjust.push_back(min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - InventoryLevel[CustomerStockoutTime[StockoutCustomer][0]][j - 1], 0.0)));
                                        // cout << "min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin - InventoryLevel[CustomerStockoutTime[StockoutCustomer][0]][j - 1], 0.0)): " << min(fabs(CustomerStockout[StockoutCustomer]), max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][j] - IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryBegin - InventoryLevel[CustomerStockoutTime[StockoutCustomer][0]][j - 1], 0.0)) << endl;
                                    }
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
                        // cout << "Customer " << StockoutCustomerSchedule[i][j][0] << " is visited at time " << StockoutCustomerSchedule[i][j][1] << " via vehicle " << StockoutCustomerSchedule[i][j][2] << " with adjust quantity " << QuantityCanAdjust[i][j] << endl;
                    }
                    // cout << "------------------------------" << endl;
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
                                if (i == StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0])
                                {
                                    ThisCustomerIsVisitedAtTheSameTimeWithStockoutCustomer = false; // Excluding stockout customers
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
                        // cout << "For stockout customer " << StockoutCustomerSchedule[i][j][0] << ", at time " << StockoutCustomerSchedule[i][j][1] << ", via vehicle " << StockoutCustomerSchedule[i][j][2] << ", the following customers can adjust their delivery quantity: " << endl;
                        for (int k = 0; k < CustomerCanAdjust[i][j].size(); k++)
                        {
                            // cout << "Customer " << CustomerCanAdjust[i][j][k][0] << ", at day " << CustomerCanAdjust[i][j][k][1] << endl;
                        }
                        // cout << "------------------------------" << endl;
                    }
                }
                for (int StockoutCustomer = 0; StockoutCustomer < CustomerCanAdjust.size(); StockoutCustomer++)
                {
                    try
                    {
                        for (int StockoutCustomerVisit = 0; StockoutCustomerVisit < CustomerCanAdjust[StockoutCustomer].size(); StockoutCustomerVisit++)
                        {

                            SlackIsZero = true; // If slack is available for at least one customer.
                            for (int CanAdjustCustomer = 0; CanAdjustCustomer < CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit].size(); CanAdjustCustomer++)
                            {

                                int CustomerIDCanAdjust = CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit][CanAdjustCustomer][0];
                                int TimeOfThisVisit = CustomerCanAdjust[StockoutCustomer][StockoutCustomerVisit][CanAdjustCustomer][1];
                                int VehicleOfThisVisit = VehicleAllocation[CustomerIDCanAdjust][TimeOfThisVisit];
                                bool SlackAvailable = false;

                                // cout << "check if there is slack in any of the previous visits for customer " << CustomerIDCanAdjust << endl;
                                for (int i = 0; i < TimeOfThisVisit; i++) // check if there is slack in any of the previous visits
                                {
                                    if (VehicleAllocation[CustomerIDCanAdjust][i] < IRPLR.NumberOfVehicles) // If this is visited, check if there is slack
                                    {
                                        // cout << "IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax: " << IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax<<", DeliveryQuantity[CustomerIDCanAdjust][i]:" << DeliveryQuantity[CustomerIDCanAdjust][i] << ", IRPLR.Vehicle.capacity:" << IRPLR.Vehicle.capacity<<", VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]]:" << VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]] << endl;

                                        double PreviousInventory = 0;
                                        if (i == 0)
                                        {
                                            PreviousInventory = max(0.0, IRPLR.Retailers[CustomerIDCanAdjust].InventoryBegin);
                                        }
                                        else
                                        {
                                            PreviousInventory = max(0.0, InventoryLevel[CustomerIDCanAdjust][i - 1]);
                                        }
                                        double AvailableInventorySlack = max(IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - PreviousInventory - DeliveryQuantity[CustomerIDCanAdjust][i], 0.0);
                                        // cout << "AvailableInventorySlack based on inventory limit for customer " << CustomerIDCanAdjust << " at time " << i << " is " << AvailableInventorySlack << endl;

                                        // cout << "IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax:" << IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax << ", PreviousInventory:" << PreviousInventory << ", DeliveryQuantity[CustomerIDCanAdjust][i]:" << DeliveryQuantity[CustomerIDCanAdjust][i] << endl;
                                        double AvailableSlack = min(AvailableInventorySlack, IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]]);
                                        //  cout << "available slack for customer " << CustomerIDCanAdjust << " at time " << i << " is " << AvailableSlack << " delivery quantity:" << DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] << endl;
                                        AvailableSlack = min(AvailableSlack, DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit]); // Cannot adjust more than the delivery quantity at TimeOfThisVisit

                                        if (AvailableSlack > 0.00001)
                                        {

                                            SlackIsZero = false;
                                            double AdjustQuantity = min(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit], AvailableSlack);
                                            DeliveryQuantity[CustomerIDCanAdjust][i] += AdjustQuantity;
                                            VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]] += AdjustQuantity;
                                            DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] -= AdjustQuantity;
                                            VehicleLoad[TimeOfThisVisit][VehicleAllocation[CustomerIDCanAdjust][TimeOfThisVisit]] -= AdjustQuantity;

                                            // cout <<     "Customer " << CustomerIDCanAdjust << " has a visit at time " << i << " via vehicle " << VehicleAllocation[CustomerIDCanAdjust][i] << " with adjustable quantity " << AdjustQuantity << endl;
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
                                            double ChangeInTotalQuantity = 0;
                                            double NewStockOut = 0;

                                            if (StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] == 0)
                                            {
                                                AdjustQuantityAndInventoryLevel(
                                                    IRPLR.Retailers[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].InventoryBegin,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1],
                                                    VehicleAllocation[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]],
                                                    DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    VehicleLoad,
                                                    VehicleAllocation,
                                                    ChangeInTotalQuantity,
                                                    NewStockOut,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0],
                                                    IRPLR);
                                            }
                                            else
                                            {

                                                AdjustQuantityAndInventoryLevel(
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] - 1],
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1],
                                                    VehicleAllocation[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]],
                                                    DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    VehicleLoad,
                                                    VehicleAllocation,
                                                    ChangeInTotalQuantity,
                                                    NewStockOut,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0],
                                                    IRPLR);
                                            }
                                            QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] = min(fabs(CustomerStockout[StockoutCustomer]) - AdjustQuantity, max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit], 0.0));
                                            // cout << "fabs(CustomerStockout[StockoutCustomer]):" << fabs(CustomerStockout[StockoutCustomer]) << ", AdjustQuantity: " << AdjustQuantity << ", IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax:" << IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax << ", DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit]: " << DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit] << endl;
                                            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
                                            assert(DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit] <= IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax);
                                            // cout << "Check if stockout is resolved for customer " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0] << " QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] " << QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] << endl;

                                            bool StockoutResolvedAtTimeOfThisVisit = true;
                                            if (InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][TimeOfThisVisit] < -0.00001)
                                            {
                                                StockoutResolvedAtTimeOfThisVisit = false;
                                            }
                                            if (StockoutResolvedAtTimeOfThisVisit == true)
                                            {
                                                // assert(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] == 0);
                                                throw TimeOfThisVisit;
                                            }
                                        }
                                    }
                                }
                                // cout << "check if there is slack in any of the future visits for customer " << CustomerIDCanAdjust << endl;
                                for (int i = TimeOfThisVisit + 1; i < IRPLR.TimeHorizon; i++) // check if there is slack in any of the future visits
                                {
                                    if (VehicleAllocation[CustomerIDCanAdjust][i] < IRPLR.NumberOfVehicles) // If this is visited, check if there is slack
                                    {

                                        // cout << "QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit]:" << QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] << ", DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit]:" << DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] << ", InventoryLevel:" << InventoryLevel[CustomerIDCanAdjust][TimeOfThisVisit] << endl;

                                        double Inventory = max(0.0, InventoryLevel[CustomerIDCanAdjust][TimeOfThisVisit]);
                                        double QuantityCanAdjustInFutureVisit = min(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit], min(DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit], Inventory));
                                        double LargestStockout = 0;
                                        double tempInventoryBegin = 0;
                                        if (TimeOfThisVisit == 0)
                                        {
                                            tempInventoryBegin = IRPLR.Retailers[CustomerIDCanAdjust].InventoryBegin - QuantityCanAdjustInFutureVisit;
                                        }
                                        else
                                        {
                                            tempInventoryBegin = InventoryLevel[CustomerIDCanAdjust][TimeOfThisVisit - 1] - QuantityCanAdjustInFutureVisit;
                                        }

                                        // cout << "Inventory:" << tempInventoryBegin << ", LargestStockout:" << LargestStockout << endl;
                                        for (int y = TimeOfThisVisit; y < i; y++)
                                        {
                                            tempInventoryBegin = tempInventoryBegin - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                                            if (tempInventoryBegin < -0.00001)
                                            {
                                                if (LargestStockout > tempInventoryBegin)
                                                {
                                                    LargestStockout = tempInventoryBegin;
                                                }
                                            }

                                            // cout <<     "Time " << y << ", Inventory:" << tempInventoryBegin << ", LargestStockout:" << LargestStockout << endl;
                                        }

                                        double AvailableInventorySlack = max(IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax - tempInventoryBegin - DeliveryQuantity[CustomerIDCanAdjust][i], 0.0);
                                        // cout<<"IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax:" << IRPLR.Retailers[CustomerIDCanAdjust].InventoryMax << ", tempInventoryBegin:" << tempInventoryBegin << ", DeliveryQuantity[CustomerIDCanAdjust][i]:" << DeliveryQuantity[CustomerIDCanAdjust][i] << endl;
                                        // cout<<"AvailableInventorySlack:"<< AvailableInventorySlack << endl;
                                        if (LargestStockout < -0.00001)
                                        {
                                            LargestStockout = -LargestStockout;
                                            QuantityCanAdjustInFutureVisit = max(0.0, QuantityCanAdjustInFutureVisit - LargestStockout);
                                        }

                                        QuantityCanAdjustInFutureVisit = min(AvailableInventorySlack, QuantityCanAdjustInFutureVisit);

                                        double AvailableSlack = min(QuantityCanAdjustInFutureVisit, IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]]);
                                        //  cout << "available slack for customer " << CustomerIDCanAdjust << " at time " << i << " is " << AvailableSlack << endl;

                                        if (AvailableSlack > 0.00001)
                                        {

                                            SlackIsZero = false;
                                            DeliveryQuantity[CustomerIDCanAdjust][i] += AvailableSlack;
                                            VehicleLoad[i][VehicleAllocation[CustomerIDCanAdjust][i]] += AvailableSlack;
                                            DeliveryQuantity[CustomerIDCanAdjust][TimeOfThisVisit] -= AvailableSlack;
                                            VehicleLoad[TimeOfThisVisit][VehicleAllocation[CustomerIDCanAdjust][TimeOfThisVisit]] -= AvailableSlack;

                                            if (TimeOfThisVisit == 0)
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

                                                double tempInventory = InventoryLevel[CustomerIDCanAdjust][TimeOfThisVisit - 1];
                                                for (int y = TimeOfThisVisit; y < InventoryLevel[CustomerIDCanAdjust].size(); y++)
                                                {
                                                    tempInventory = tempInventory - IRPLR.Retailers[CustomerIDCanAdjust].Demand + DeliveryQuantity[CustomerIDCanAdjust][y];
                                                    InventoryLevel[CustomerIDCanAdjust][y] = tempInventory;
                                                }
                                            }
                                            DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]] += AvailableSlack;
                                            VehicleLoad[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][2]] += AvailableSlack;
                                            double ChangeInTotalQuantity = 0;
                                            double NewStockOut = 0;
                                            if (StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] == 0)
                                            {

                                                AdjustQuantityAndInventoryLevel(
                                                    IRPLR.Retailers[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]].InventoryBegin,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1],
                                                    VehicleAllocation[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]],
                                                    DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    VehicleLoad,
                                                    VehicleAllocation,
                                                    ChangeInTotalQuantity,
                                                    NewStockOut,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0],
                                                    IRPLR);
                                            }
                                            else
                                            {
                                                AdjustQuantityAndInventoryLevel(
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1] - 1],
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1],
                                                    VehicleAllocation[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][1]],
                                                    DeliveryQuantity[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]],
                                                    VehicleLoad,
                                                    VehicleAllocation,
                                                    ChangeInTotalQuantity,
                                                    NewStockOut,
                                                    StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0],
                                                    IRPLR);
                                            }
                                            QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] = min(fabs(CustomerStockout[StockoutCustomer]) - AvailableSlack, max(IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax - DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit], 0.0));
                                            // cout << "fabs(CustomerStockout[StockoutCustomer]):" << fabs(CustomerStockout[StockoutCustomer]) << ", AvailableSlack: " << AvailableSlack << ", IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax:" << IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax << ", DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit]: " << DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit] << endl;
                                            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
                                            assert(DeliveryQuantity[CustomerStockoutTime[StockoutCustomer][0]][TimeOfThisVisit] <= IRPLR.Retailers[CustomerStockoutTime[StockoutCustomer][0]].InventoryMax);
                                            // cout << "Check if stockout is resolved for customer " << StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0] << " QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] " << QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] << endl;

                                            bool StockoutResolvedAtTimeOfThisVisit = true;
                                            if (InventoryLevel[StockoutCustomerSchedule[StockoutCustomer][StockoutCustomerVisit][0]][TimeOfThisVisit] < -0.00001)
                                            {
                                                StockoutResolvedAtTimeOfThisVisit = false;
                                            }
                                            if (StockoutResolvedAtTimeOfThisVisit == true)
                                            {
                                                // assert(QuantityCanAdjust[StockoutCustomer][StockoutCustomerVisit] == 0);
                                                throw TimeOfThisVisit;
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
                    catch (int TimeOfThisVisit)
                    {
                        // cout << "Stockout is resolved for this customer at time " << TimeOfThisVisit << endl;
                    }
                    // if (StockoutResolved == true)
                    // {
                    //     break;
                    // }
                }
            }
        }
        for (int customer = 0; customer < InventoryLevel.size(); customer++)
        {
            for (int time = 0; time < InventoryLevel[customer].size(); time++)
            {
                if (InventoryLevel[customer][time] < -0.00001)
                {
                    // cout << "Customer " << customer << " has negative inventory level " << InventoryLevel[customer][time] << " at time " << time << endl;
                    // assert(InventoryLevel[customer][time] > 0.00001);
                    throw InventoryLevel[customer][time];
                }
            }
        }
        cout << "rebalance operator successfully repaired the solution." << endl;
        // ///////////////////////////////////////////////////////////////
        // //                                                           //
        // //       Remove visits from the last day if possible         //
        // //                                                           //
        // ///////////////////////////////////////////////////////////////

        // for (int customer = 0; customer < DeliveryQuantity.size(); customer++)
        // {
        //     if (VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1] < IRPLR.NumberOfVehicles)
        //     {
        //         if (fabs(InventoryLevel[customer][InventoryLevel[customer].size() - 1] - DeliveryQuantity[customer][DeliveryQuantity[customer].size() - 1]) < 0.00001)
        //         {
        //             cout << "Customer " << customer << " has a visit at the last day with delivery quantity " << DeliveryQuantity[customer][DeliveryQuantity[customer].size() - 1] << " and inventory level " << InventoryLevel[customer][InventoryLevel[customer].size() - 1] << ", remove is possible." << endl;
        //             double deduct = DeliveryQuantity[customer][DeliveryQuantity[customer].size() - 1];
        //             InventoryLevel[customer][InventoryLevel[customer].size() - 1] -= deduct;
        //             DeliveryQuantity[customer][DeliveryQuantity[customer].size() - 1] -= deduct;
        //             UnallocatedCustomers[DeliveryQuantity[customer].size() - 1].push_back(Route[DeliveryQuantity[customer].size() - 1][VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1]][VisitOrder[customer][DeliveryQuantity[customer].size() - 1]]);
        //             Route[DeliveryQuantity[customer].size() - 1][VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1]].erase(Route[DeliveryQuantity[customer].size() - 1][VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1]].begin() + VisitOrder[customer][InventoryLevel[customer].size() - 1], Route[DeliveryQuantity[customer].size() - 1][VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1]].begin() + VisitOrder[customer][InventoryLevel[customer].size() - 1] + 1);
        //             VisitOrder[customer][InventoryLevel[customer].size() - 1] = IRPLR.Retailers.size() + 1;
        //             VehicleAllocation[customer][DeliveryQuantity[customer].size() - 1] = IRPLR.NumberOfVehicles + 1;
        // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
        //         }
        //     }
        // }
        ///////////////////////////////////////////////////////////////
        //                                                           //
        //                      Rebalance Stage 2                   //
        //                                                           //
        ///////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        //                                                                       //
        //     At this stage, the schedule of this time is already feasible      //
        //                                                                       //
        ///////////////////////////////////////////////////////////////////////////

        PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
        for (int time = 0; time < IRPLR.TimeHorizon; time++)
        {

            ///////////////////////////////////////////////////////////////////////////
            //                                                                       //
            //           The following code removes unnecessary visits               //
            //                  to avoid high travel costs.                          //
            //                                                                       //
            ///////////////////////////////////////////////////////////////////////////

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
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);

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
                cout << "---------" << endl;

                PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
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

                double DeliverMore = 0;
                if (time == 0)
                {
                    assert(IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] <= IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax + 0.00001);
                    DeliverMore = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
                                      IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin - DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time]);
                    // cout << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin
                    //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax
                    //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin << endl;
                    // cout << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]]
                    //      << "," << IRPLR.Vehicle.capacity
                    //      << "," << VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] << endl;
                }
                else
                {
                    // cout << "InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1]:" << InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] << ", DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time]:" << DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] << ", IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax:" << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax << endl;
                    assert(InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] + DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] <= IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax + 0.00001);

                    DeliverMore = min(IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]],
                                      IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] - DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time]);
                    // cout << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax - InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1]
                    //      << "," << IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryMax
                    //      << "," << InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1] << endl;
                    // cout << IRPLR.Vehicle.capacity - VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]]
                    //      << "," << IRPLR.Vehicle.capacity
                    //      << "," << VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] << endl;
                }

                assert(DeliverMore >= 0);
                DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] += DeliverMore;
                VehicleLoad[time][VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time]] += DeliverMore;

                assert(DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]][time] >= 0);
                double ChangeInTotalQuantity = 0;
                double NewStockOut = 0;
                if (time == 0)
                {
                    AdjustQuantityAndInventoryLevel(
                        IRPLR.Retailers[CumulativeCustomerWeight[RandomCustomer][0]].InventoryBegin,
                        time,
                        VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time],
                        DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]],
                        InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]],
                        VehicleLoad,
                        VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        CumulativeCustomerWeight[RandomCustomer][0],
                        IRPLR);
                }
                else
                {
                    AdjustQuantityAndInventoryLevel(
                        InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]][time - 1],
                        time,
                        VehicleAllocation[CumulativeCustomerWeight[RandomCustomer][0]][time],
                        DeliveryQuantity[CumulativeCustomerWeight[RandomCustomer][0]],
                        InventoryLevel[CumulativeCustomerWeight[RandomCustomer][0]],
                        VehicleLoad,
                        VehicleAllocation,
                        ChangeInTotalQuantity,
                        NewStockOut,
                        CumulativeCustomerWeight[RandomCustomer][0],
                        IRPLR);
                }

                CustomerWeight.erase(CustomerWeight.begin() + RandomCustomer, CustomerWeight.begin() + RandomCustomer + 1);
                CumulativeCustomerWeight.erase(CumulativeCustomerWeight.begin() + RandomCustomer, CumulativeCustomerWeight.begin() + RandomCustomer + 1);
            }
            // cout << "After complete rebalancing, time " << time << endl;
            // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);

            // assert(aborter == 1);
        }

        // ///////////////////////////////////////////////////////////////////////////
        // //                                                                       //
        // //           The following code removes unnecessary visits               //
        // //                  to avoid high travel costs.                          //
        // //                                                                       //
        // ///////////////////////////////////////////////////////////////////////////

        // for (int time = 0; time < IRPLR.TimeHorizon; time++)
        // {

        //     // Search for an existing visit that delivers nothing, find the visit in the route and delete it
        //     // Otherwise if an existing visit delivers, computes the total delivery quantity by incrementing the delivery quantity of each visit.

        //     for (int i = 0; i < DeliveryQuantity.size(); i++) // Index i for retailer
        //     {

        //         if (DeliveryQuantity[i][time] == 0 && VehicleAllocation[i][time] < IRPLR.NumberOfVehicles)
        //         {
        //             UnallocatedCustomers[time].push_back(Route[time][VehicleAllocation[i][time]][VisitOrder[i][time]]);
        //             Route[time][VehicleAllocation[i][time]].erase(Route[time][VehicleAllocation[i][time]].begin() + VisitOrder[i][time], Route[time][VehicleAllocation[i][time]].begin() + VisitOrder[i][time] + 1);

        //             for (int k = 0; k < VisitOrder.size(); k++)
        //             {
        //                 if (VehicleAllocation[k][time] == VehicleAllocation[i][time]) // Find visits in the same vehicle as Retailer i on day j
        //                 {
        //                     if (VisitOrder[k][time] > VisitOrder[i][time]) // If this visit is after the visit of Retailer i
        //                     {
        //                         VisitOrder[k][time] = VisitOrder[k][time] - 1;
        //                     }
        //                 }
        //             }
        //             VisitOrder[i][time] = IRPLR.Retailers.size() + 1;
        //             VehicleAllocation[i][time] = IRPLR.NumberOfVehicles + 1;
        //         }
        //         else
        //         {
        //             if (VehicleAllocation[i][time] == IRPLR.NumberOfVehicles + 1)
        //             {
        //                 assert(DeliveryQuantity[i][time] == 0);
        //             }
        //         }
        //     }
        // }

        // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);
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