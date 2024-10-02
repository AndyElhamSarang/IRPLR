#include "lib.h"
void solution_construction::INITIAL_Single_Vehicle(input &IRPLR, solution &IRPSolution)
{
/*
    cout << "INITIAL for IRPLR" << endl;
    cout << "Vehicle capacity:" << IRPLR.Vehicle.capacity << endl;
    cout << "Vehicles total capacity:" << IRPLR.VehiclesTotalCapacity << endl;
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
        vector<int> tempScheduleOfVisit;
        IRPSolution.SchduleOfVisit.push_back(tempScheduleOfVisit);
    }

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        IRPSolution.DailyQuantity.push_back(0);
    }

    IRPSolution.print_solution(IRPLR);

    boost_random_mechanism RandomInitial;
    vector<int> CandidateRetailers;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
        CandidateRetailers.push_back(i);

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, i);
        double ratio_capacity_vs_demand = IRPLR.Retailers[i].InventoryMax / IRPLR.Retailers[i].Demand;
        cout << "Retailer " << i << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << endl;
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
            cout << "NextStockOutPeriod:" << NextStockOutPeriod << endl;

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
                    cout << "going forward" << endl;
                    cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;

                    vector<int> PotentialPeriods;
                    for (int i = CurrentPeriod; i <= NextStockOutPeriod; i++)
                    {
                        int VisitedOrNot = 0;
                        for (int j = 0; j < IRPSolution.SchduleOfVisit[i].size(); j++)
                        {
                            if (IRPSolution.SchduleOfVisit[i][j] == CandidateRetailers[RandomPickARetailer])
                                VisitedOrNot = 1;
                            if (fabs(IRPSolution.DailyQuantity[i] - IRPLR.Vehicle.capacity) < 0.001)
                                VisitedOrNot = 1;
                        }
                        if (VisitedOrNot == 0)
                            PotentialPeriods.push_back(i);
                    }
                    cout << "PotentialPeriods:" << endl;
                    for (int i = 0; i < PotentialPeriods.size(); i++)
                        cout << PotentialPeriods[i] << ",";

                    cout << endl;
                    if (PotentialPeriods.size() == 0)
                    {
                        cout << "No Potential periods available, going backward further" << endl;
                        CurrentPeriod = NextStockOutPeriod + 1;
                    }
                    else
                    {
                        int RandomPotentialPeirod = RandomInitial.random_number_generator(0, PotentialPeriods.size() - 1, generator);
                        int RandomPickANonStockOutPeriod = PotentialPeriods[RandomPotentialPeirod];
                        cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << ", SelectedTimePeriod: " << RandomPickANonStockOutPeriod << endl;

                        IRPSolution.SchduleOfVisit[RandomPickANonStockOutPeriod].push_back(CandidateRetailers[RandomPickARetailer]);
                        double total_delivery = 0;

                        for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
                            total_delivery += IRPSolution.DeliveryQuantity[i][RandomPickANonStockOutPeriod];

                        if (RandomPickANonStockOutPeriod > 0)
                        {
                            // cout << "IRPLR.Vehicle.capacity:" << IRPLR.Vehicle.capacity << endl;
                            // cout << "total_delivery:" << total_delivery << endl;
                            // cout << "IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax:" << IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax << endl;
                            // cout << "IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]:" << IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1] << endl;
                            IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = min(IRPLR.Vehicle.capacity - total_delivery, IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]);
                        }
                        else
                        {
                            IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = min(IRPLR.Vehicle.capacity - total_delivery, IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
                        }
                        int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
                        for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
                        {
                            tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
                            IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
                        }
                        CurrentPeriod = RandomPickANonStockOutPeriod + 1;
                        // CandidateRetailers.erase(CandidateRetailers.begin() + RandomPickARetailer, CandidateRetailers.begin() + RandomPickARetailer + 1);
                        for (int i = 0; i < IRPSolution.DailyQuantity.size(); i++)
                            IRPSolution.DailyQuantity[i] = 0;
                        for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
                        {
                            for (int j = 0; j < IRPSolution.DeliveryQuantity[i].size(); j++)
                            {
                                IRPSolution.DailyQuantity[j] += IRPSolution.DeliveryQuantity[i][j];
                            }
                        }
                        IRPSolution.print_solution(IRPLR);
                    }
                }
                else
                {
                    cout << "Going backward" << endl;
                    cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;

                    int RandomPickANonStockOutPeriod = IRPLR.TimeHorizon + 1;

                    for (int i = NextStockOutPeriod - 1; i >= 0; i--)
                    {
                        cout << IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i] << "," << i + 1 << "," << NextStockOutPeriod - 1 << endl;

                        if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i] == 0)
                        {
                            if (fabs(IRPSolution.DailyQuantity[i] - IRPLR.Vehicle.capacity) > 0.001)
                            {
                                RandomPickANonStockOutPeriod = i;
                                break;
                            }
                        }
                    }
                    assert(RandomPickANonStockOutPeriod < IRPLR.TimeHorizon + 1);
                    CurrentPeriod = RandomPickANonStockOutPeriod;
                    cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << ", SelectedTimePeriod: " << RandomPickANonStockOutPeriod << endl;

                    IRPSolution.SchduleOfVisit[RandomPickANonStockOutPeriod].push_back(CandidateRetailers[RandomPickARetailer]);
                    double total_delivery = 0;
                    for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
                        total_delivery += IRPSolution.DeliveryQuantity[i][RandomPickANonStockOutPeriod];

                    if (RandomPickANonStockOutPeriod > 0)
                    {
                        // cout << "IRPLR.Vehicle.capacity:" << IRPLR.Vehicle.capacity << endl;
                        // cout << "total_delivery:" << total_delivery << endl;
                        // cout << "IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax:" << IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax << endl;
                        // cout << "IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]:" << IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1] << endl;
                        IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = min(IRPLR.Vehicle.capacity - total_delivery, IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]);
                    }
                    else
                    {
                        IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = min(IRPLR.Vehicle.capacity - total_delivery, IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
                    }
                    int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
                    for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
                    {
                        tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
                        IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
                    }
                    CurrentPeriod = RandomPickANonStockOutPeriod + 1;
                    // CandidateRetailers.erase(CandidateRetailers.begin() + RandomPickARetailer, CandidateRetailers.begin() + RandomPickARetailer + 1);
                    for (int i = 0; i < IRPSolution.DailyQuantity.size(); i++)
                        IRPSolution.DailyQuantity[i] = 0;
                    for (int i = 0; i < IRPSolution.DeliveryQuantity.size(); i++)
                    {
                        for (int j = 0; j < IRPSolution.DeliveryQuantity[i].size(); j++)
                        {
                            IRPSolution.DailyQuantity[j] += IRPSolution.DeliveryQuantity[i][j];
                        }
                    }
                    IRPSolution.print_solution(IRPLR);
                }
            }
        }
    }*/
}