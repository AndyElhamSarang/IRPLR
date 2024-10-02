#include "lib.h"
void solution_construction::Initial_Schedule(input &IRPLR, solution &IRPSolution)
{
    cout << "INITIAL for IRPLR: multi vehicle case" << endl;
    if (printout_initialSchedule == 1)
    {
        cout << "Vehicle capacity:" << IRPLR.Vehicle.capacity << endl;
        cout << "Vehicles total capacity:" << IRPLR.VehiclesTotalCapacity << endl;
    }
    // Initilize solution
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> TempDeliveryQuantity;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
            TempDeliveryQuantity.push_back(0);
        IRPSolution.DeliveryQuantity.push_back(TempDeliveryQuantity);
    }

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> tempInventoryLevel;
        int tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand + IRPSolution.DeliveryQuantity[i][j];
            tempInventoryLevel.push_back(tempInventory);
        }
        IRPSolution.InventoryLevel.push_back(tempInventoryLevel);
    }

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<double> temp_VehicleLoad;
        for (int j = 0; j < IRPLR.NumberOfVehicles; j++)
        {
            temp_VehicleLoad.push_back(0);
        }
        IRPSolution.VehicleLoad.push_back(temp_VehicleLoad);
    }
    IRPSolution.Route.clear();
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<vector<int>> temp_Route;
        for (int j = 0; j < IRPLR.NumberOfVehicles; j++)
        {
            vector<int> temp_temp_route;
            temp_Route.push_back(temp_temp_route);
        }
        IRPSolution.Route.push_back(temp_Route);
    }
    if(printout_initialSchedule==1){IRPSolution.print_solution(IRPLR);}

    boost_random_mechanism RandomInitial;
    vector<int> CandidateRetailers;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
        CandidateRetailers.push_back(i);

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, i);
        double ratio_capacity_vs_demand = IRPLR.Retailers[i].InventoryMax / IRPLR.Retailers[i].Demand;
        if (printout_initialSchedule == 1)
        {
            cout << "Retailer " << i << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << endl;
        }
    }

    while (CandidateRetailers.size() != 0)
    {
        int RandomPickARetailer = RandomInitial.random_number_generator(0, CandidateRetailers.size() - 1, generator);
        // cout<<CandidateRetailers[RandomPickARetailer]<<","<<IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].ID<<","<<IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].xCoord<<","<<IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].yCoord<<endl;
        int StockOutExist = 0;
        int CurrentPeriod = 0;
        int NextStockOutPeriod = 0;
        while (NextStockOutPeriod < IRPLR.TimeHorizon + 1)
        {
            NextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, CandidateRetailers[RandomPickARetailer]);
            if (printout_initialSchedule == 1)
            {
                cout << "NextStockOutPeriod:" << NextStockOutPeriod << endl;
            }

            if (NextStockOutPeriod >= IRPLR.TimeHorizon + 1)
            {
                // No further stock out
                CandidateRetailers.erase(CandidateRetailers.begin() + RandomPickARetailer, CandidateRetailers.begin() + RandomPickARetailer + 1);
                CurrentPeriod = IRPLR.TimeHorizon + 1;
                break;
            }
            else
            {
                if (CurrentPeriod <= NextStockOutPeriod)
                {
                    if (printout_initialSchedule == 1)
                    {
                        cout << "going forward" << endl;
                        cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;
                    }

                    vector<int> PotentialPeriods;
                    for (int i = CurrentPeriod; i <= NextStockOutPeriod; i++)
                    {
                        int VisitedOrNot = 0;
                        double TotalLoad = 0;
                        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
                        {
                            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
                            {
                                if (IRPSolution.Route[i][j][k] == CandidateRetailers[RandomPickARetailer])
                                    VisitedOrNot = 1;
                            }
                            TotalLoad += IRPSolution.VehicleLoad[i][j];
                        }
                        if (fabs(TotalLoad - IRPLR.Vehicle.capacity * IRPLR.NumberOfVehicles) < 0.001)
                            VisitedOrNot = 1;
                        if (VisitedOrNot == 0)
                            PotentialPeriods.push_back(i);
                    }
                    if (printout_initialSchedule == 1)
                    {
                        cout << "PotentialPeriods:" << endl;
                        for (int i = 0; i < PotentialPeriods.size(); i++)
                            cout << PotentialPeriods[i] << ",";
                        cout << endl;
                    }
                    if (PotentialPeriods.size() == 0)
                    {
                        if (printout_initialSchedule == 1)
                        {
                            cout << "No Potential periods available, going backward further" << endl;
                        }
                        CurrentPeriod = NextStockOutPeriod + 1;
                    }
                    else
                    {
                        int RandomPotentialPeirod = RandomInitial.random_number_generator(0, PotentialPeriods.size() - 1, generator);
                        int RandomPickANonStockOutPeriod = PotentialPeriods[RandomPotentialPeirod];
                        if (printout_initialSchedule == 1)
                        {
                            cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << ", SelectedTimePeriod: " << RandomPickANonStockOutPeriod << endl;
                        }

                        if (RandomPickANonStockOutPeriod > 0)
                        {
                            // cout << "IRPLR.Vehicle.capacity:" << IRPLR.Vehicle.capacity << endl;
                            // cout << "total_delivery:" << total_delivery << endl;
                            // cout << "IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax:" << IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax << endl;
                            // cout << "IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]:" << IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1] << endl;
                            double Load = 0;
                            int vehicle_index = IRPLR.NumberOfVehicles;
                            for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
                            {
                                Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]);
                                if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] < Load)
                                {
                                    IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = Load;
                                    vehicle_index = i;
                                }
                            }
                            IRPSolution.Route[RandomPickANonStockOutPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
                            IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod];
                        }
                        else
                        {
                            double Load = 0;
                            int vehicle_index = IRPLR.NumberOfVehicles;
                            for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
                            {
                                Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
                                if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] < Load)
                                {
                                    IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = Load;
                                    vehicle_index = i;
                                }
                            }
                            IRPSolution.Route[RandomPickANonStockOutPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
                            IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod];
                        }
                        int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
                        for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
                            IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
                        }
                        CurrentPeriod = RandomPickANonStockOutPeriod + 1;

                        if(printout_initialSchedule==1){IRPSolution.print_solution(IRPLR);}
                    }
                }
                else
                {
                    if (printout_initialSchedule == 1)
                    {
                        cout << "Going backward" << endl;
                        cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;
                    }

                    int LookBackwardPeriod = IRPLR.TimeHorizon + 1;

                    for (int i = NextStockOutPeriod - 1; i >= 0; i--)
                    {
                        if (printout_initialSchedule == 1)
                        {
                            cout << IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i] << "," << i + 1 << "," << NextStockOutPeriod - 1 << endl;
                        }
                        int VisitedOrNot = 0;
                        double TotalLoad = 0;
                        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
                        {
                            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
                            {
                                if (IRPSolution.Route[i][j][k] == CandidateRetailers[RandomPickARetailer])
                                    VisitedOrNot = 1;
                            }
                            TotalLoad += IRPSolution.VehicleLoad[i][j];
                        }
                        if (fabs(TotalLoad - IRPLR.Vehicle.capacity * IRPLR.NumberOfVehicles) < 0.001)
                            VisitedOrNot = 1;
                        if (VisitedOrNot == 0)
                        {
                            LookBackwardPeriod = i;
                            break;
                        }
                    }
                    assert(LookBackwardPeriod < IRPLR.TimeHorizon + 1);
                    CurrentPeriod = LookBackwardPeriod;
                    if (printout_initialSchedule == 1)
                    {
                        cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << ", SelectedTimePeriod: " << LookBackwardPeriod << endl;
                    }

                    if (LookBackwardPeriod > 0)
                    {
                        double Load = 0;
                        int vehicle_index = IRPLR.NumberOfVehicles;
                        for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
                        {
                            Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod - 1]);
                            if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
                            {
                                IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
                                vehicle_index = i;
                            }
                        }
                        IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
                        IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
                    }
                    else
                    {
                        double Load = 0;
                        int vehicle_index = IRPLR.NumberOfVehicles;
                        for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
                        {
                            Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
                            if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
                            {
                                IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
                                vehicle_index = i;
                            }
                        }
                        IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
                        IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
                    }
                    int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
                    for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
                    {
                        tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
                        IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
                    }
                    CurrentPeriod = LookBackwardPeriod + 1;
                    if(printout_initialSchedule==1){IRPSolution.print_solution(IRPLR);}
                }
            }
        }
    }
    assert(IRPSolution.Route.size() == IRPLR.TimeHorizon);
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        assert(IRPSolution.Route[i].size() == IRPLR.NumberOfVehicles);
    }
}