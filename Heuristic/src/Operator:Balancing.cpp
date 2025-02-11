#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR, preprocessing &memory,
                                               vector<vector<vector<int>>> &Route,//Stay fixed
                                               vector<vector<int>> &UnallocatedCustomers, //Stay fixed
                                               vector<vector<double>> &VehicleLoad,
                                               vector<vector<double>> &DeliveryQuantity,
                                               vector<vector<double>> &InventoryLevel,
                                               vector<vector<int>> &VehicleAllocation//Stay fixed
                                               )
{
    cout << "Balancing quantity operator" << endl;

    vector<int>NumberOfVisits;
    //Reset the solution
    vector<vector<double>> WeightedProbability(memory.CustomerPriority);
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

         double tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < DeliveryQuantity[i].size(); j++)
        {
            DeliveryQuantity[i][j] = 0;
            InventoryLevel[i][j] = tempInventory - IRPLR.Retailers[i].Demand;;
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand;
        }
        AllCustomers.push_back(i);
    }

    for(int i=0;i<UnallocatedCustomers.size();i++)
    {
        for(int j=0;j<UnallocatedCustomers[i].size();j++)
        {
            cout<<UnallocatedCustomers[i][j]<<",";
            
            WeightedProbability[UnallocatedCustomers[i][j]][i] = 0;
        }
    }
    cout<<endl;
    int aborter=0;
    cout << "Weighted probability" << endl;
    cout << '\t' << '\t';
    for (int z = 0; z < IRPLR.TimeHorizon; z++)
    {
        cout << "t" << z << "\t";
    }
    cout << endl;
    for (int x = 0; x < WeightedProbability.size(); x++)
    {
        cout << "Retailer " << x << ": " << '\t';

        for (int y = 0; y < WeightedProbability[x].size() - 1; y++)
        {
            if (fabs(WeightedProbability[x][y] - 0) > 0.001)
            {
                cout << WeightedProbability[x][y] << '\t';
            }
            else
            {
                cout << "-" << '\t';
            }
        }
        if (fabs(WeightedProbability[x][WeightedProbability[x].size() - 1] - 0) > 0.001)
        {
            cout << WeightedProbability[x][WeightedProbability[x].size() - 1] << endl;
        }
        else
        {
            cout << "-" << endl;
        }
    }
    cout << endl;
for (int x = 0; x < WeightedProbability.size(); x++)
    {
        int TempNumberOfVisit=0;
        for (int y = 0; y < WeightedProbability[x].size(); y++)
        {
               if(fabs(WeightedProbability[x][y]-0)>0.001)
            {
                TempNumberOfVisit++;
            }
        }
        NumberOfVisits.push_back(TempNumberOfVisit);
    }
    cout << "Updating weight based on next visits except for last day"<<endl;
     for (int x = 0; x < WeightedProbability.size(); x++)
    {
        for (int y = 0; y < WeightedProbability[x].size()-1; y++)
        {
            int visited_next_day = 1;
            for (int i = 0; i < UnallocatedCustomers[y+1].size(); i++)
            {
                if (UnallocatedCustomers[y+1][i] == x)
                {
                    visited_next_day = 0;
                }
            }

            if (visited_next_day == 1)
            {
                WeightedProbability[x][y] = WeightedProbability[x][y] / 2;
            }

           
        }
    } 
    cout << "Weighted probability" << endl;
    cout << '\t' << '\t';
    for (int z = 0; z < IRPLR.TimeHorizon; z++)
    {
        cout << "t" << z << "\t";
    }
    cout << endl;
    for (int x = 0; x < WeightedProbability.size(); x++)
    {
        cout << "Retailer " << x << ": " << '\t';

        for (int y = 0; y < WeightedProbability[x].size() - 1; y++)
        {
            if (fabs(WeightedProbability[x][y] - 0) > 0.001)
            {
                cout << WeightedProbability[x][y] << '\t';
            }
            else
            {
                cout << "-" << '\t';
            }
        }
        if (fabs(WeightedProbability[x][WeightedProbability[x].size() - 1] - 0) > 0.001)
        {
            cout << WeightedProbability[x][WeightedProbability[x].size() - 1] << endl;
        }
        else
        {
            cout << "-" << endl;
        }
    }
    cout << endl;
    cout << "Updating weight based on number of visits" << endl;
    cout<<"Number of visits"<<endl;
    for(int i=0;i<NumberOfVisits.size();i++)
    {
        cout<<NumberOfVisits[i]<<",";
    }
    cout<<endl;
    for (int x = 0; x < WeightedProbability.size(); x++)
    {
        for (int y = 0; y < WeightedProbability[x].size(); y++)
        {
            cout<<(1 - NumberOfVisits[x] / (IRPLR.TimeHorizon+1))<<",";
            WeightedProbability[x][y] = WeightedProbability[x][y] * (1 - (NumberOfVisits[x] / (IRPLR.TimeHorizon+1)));
        }
    }
    cout<<endl;
    cout << "Weighted probability" << endl;
    cout << '\t' << '\t';
    for (int z = 0; z < IRPLR.TimeHorizon; z++)
    {
        cout << "t" << z << "\t";
    }
    cout << endl;
    for (int x = 0; x < WeightedProbability.size(); x++)
    {
        cout << "Retailer " << x << ": " << '\t';

        for (int y = 0; y < WeightedProbability[x].size() - 1; y++)
        {
            if (fabs(WeightedProbability[x][y] - 0) > 0.001)
            {
                cout << WeightedProbability[x][y] << '\t';
            }
            else
            {
                cout << "-" << '\t';
            }
        }
        if (fabs(WeightedProbability[x][WeightedProbability[x].size() - 1] - 0) > 0.001)
        {
            cout << WeightedProbability[x][WeightedProbability[x].size() - 1] << endl;
        }
        else
        {
            cout << "-" << endl;
        }
    }
    cout << endl;

    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    boost_random_mechanism RandomnessInBalance;
    assert(AllCustomers.size() != 0);

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout<<"Weighted probability"<<endl;
        cout << '\t'<<'\t';
        for (int z = 0; z < IRPLR.TimeHorizon; z++)
        {
            cout << "t" << z << "\t";
        }
        cout<<endl;
        for (int x = 0; x < WeightedProbability.size(); x++)
        {
            cout << "Retailer " << x <<": "<<'\t' ;
       
            for (int y = 0; y < WeightedProbability[x].size()-1; y++)
            {
                if(fabs(WeightedProbability[x][y]-0)>0.001)
                {
                cout << WeightedProbability[x][y]<<'\t';
                }
                else
                {
                    cout << "-"<<'\t';
                }
            }
             if(fabs(WeightedProbability[x][WeightedProbability[x].size() - 1]-0)>0.001)
                {
                cout << WeightedProbability[x][WeightedProbability[x].size() - 1]<<endl;
                }
                else
                {
                    cout << "-"<<endl;
                }           
        }
        cout<<endl;


        vector<int> PotentialCustomers(AllCustomers);
        while (PotentialCustomers.size() != 0)
        {
            int RandomCustomer = RandomnessInBalance.random_number_generator(0, PotentialCustomers.size() - 1, generator);
            if (VehicleAllocation[PotentialCustomers[RandomCustomer]][i] != IRPLR.NumberOfVehicles + 1) // if this customer is visited on this day
            {
                DeliveryQuantity[PotentialCustomers[RandomCustomer]][i] =
                    min(IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[PotentialCustomers[RandomCustomer]][i]],
                        IRPLR.Retailers[PotentialCustomers[RandomCustomer]].InventoryMax - InventoryLevel[PotentialCustomers[RandomCustomer]][i]);
                VehicleLoad[i][VehicleAllocation[PotentialCustomers[RandomCustomer]][i]] += DeliveryQuantity[PotentialCustomers[RandomCustomer]][i];

                if (i == 0)
                {
                    double tempInventory = IRPLR.Retailers[PotentialCustomers[RandomCustomer]].InventoryBegin;
                    for (int y = 0; y < InventoryLevel[PotentialCustomers[RandomCustomer]].size(); y++)
                    {
                        
                        tempInventory = tempInventory - IRPLR.Retailers[PotentialCustomers[RandomCustomer]].Demand + DeliveryQuantity[PotentialCustomers[RandomCustomer]][y];
                        InventoryLevel[PotentialCustomers[RandomCustomer]][y] = tempInventory;
                    }
                }
                else
                {

                    double tempInventory = InventoryLevel[PotentialCustomers[RandomCustomer]][i - 1];
                    for (int y = i; y < InventoryLevel[PotentialCustomers[RandomCustomer]].size(); y++)
                    {                        
                        tempInventory = tempInventory - IRPLR.Retailers[PotentialCustomers[RandomCustomer]].Demand + DeliveryQuantity[PotentialCustomers[RandomCustomer]][y];
                        InventoryLevel[PotentialCustomers[RandomCustomer]][y] = tempInventory;
                    }
                }

                PotentialCustomers.erase(PotentialCustomers.begin() + RandomCustomer, PotentialCustomers.begin() + RandomCustomer + 1);
            }
            else
            {
                PotentialCustomers.erase(PotentialCustomers.begin() + RandomCustomer, PotentialCustomers.begin() + RandomCustomer + 1);
            }
        }
        
    }
    cout << "End balancing" << endl;
    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    assert(aborter==1);
    // if (LookBackwardPeriod > 0)
    // {
    //     double Load = 0;
    //     int vehicle_index = IRPLR.NumberOfVehicles;
    //     for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
    //     {
    //         Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod - 1]);
    //         if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
    //         {
    //             IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
    //             vehicle_index = i;
    //         }
    //     }
    //     IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
    //     IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
    //     IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
    // }
    // else
    // {
    //     double Load = 0;
    //     int vehicle_index = IRPLR.NumberOfVehicles;
    //     for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
    //     {
    //         Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
    //         if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
    //         {
    //             IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
    //             vehicle_index = i;
    //         }
    //     }
    //     IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
    //     IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
    //     IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
    // }
    // int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
    // for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
    // {
    //     tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
    //     IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
    // }
    cout << "Exit balancing" << endl;
    return objv;
}