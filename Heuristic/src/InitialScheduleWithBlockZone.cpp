#include "lib.h"
void solution_construction::Initial_BlockZone_Schedule(input &IRPLR, solution &IRPSolution)
{
    if (printout_initialSchedule == 1)
    {
        cout << "INITIAL for IRPLR: multi vehicle case" << endl;
        cout << "Create schedule with block type zone" << endl;
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
    if (printout_initialSchedule == 1)
    {
        IRPSolution.print_solution(IRPLR);
    }

    boost_random_mechanism RandomInitial;

    vector<vector<int>> GroupOfRetailers;

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<int> TempGroupOfRetailers;
        GroupOfRetailers.push_back(TempGroupOfRetailers);
    }

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, i);
        double ratio_capacity_vs_demand = IRPLR.Retailers[i].InventoryMax / IRPLR.Retailers[i].Demand;
        if (printout_initialSchedule == 1)
        {
            cout << "Retailer " << i << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << endl;
        }
        GroupOfRetailers[checkNextStockOutPeriod].push_back(i);
    }
    if (printout_initialSchedule == 1)
    {
        cout << "Sorted via stock out period" << endl;
    }
    int GroupOfRetailersCounter = 0;
    for (int i = 0; i < GroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < GroupOfRetailers[i].size(); j++)
        {
            int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, GroupOfRetailers[i][j]);
            double ratio_capacity_vs_demand = IRPLR.Retailers[GroupOfRetailers[i][j]].InventoryMax / IRPLR.Retailers[GroupOfRetailers[i][j]].Demand;
            if (printout_initialSchedule == 1)
            {
                cout << "Retailer " << GroupOfRetailers[i][j] << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << ", xCoord:" << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << ", yCoord:" << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << endl;
            }
            GroupOfRetailersCounter++;
        }
    }
    assert(GroupOfRetailersCounter == IRPLR.Retailers.size());
    int MaxYCoord = IRPLR.Supplier.yCoord;
    int MinYCoord = IRPLR.Supplier.yCoord;
    int MaxXCoord = IRPLR.Supplier.xCoord;
    int MinXCoord = IRPLR.Supplier.xCoord;

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        if (IRPLR.Retailers[i].xCoord < MinXCoord)
        {
            MinXCoord = IRPLR.Retailers[i].xCoord;
        }
        if (IRPLR.Retailers[i].xCoord > MaxXCoord)
        {
            MaxXCoord = IRPLR.Retailers[i].xCoord;
        }
        if (IRPLR.Retailers[i].yCoord < MinYCoord)
        {
            MinYCoord = IRPLR.Retailers[i].yCoord;
        }
        if (IRPLR.Retailers[i].yCoord > MaxYCoord)
        {
            MaxYCoord = IRPLR.Retailers[i].yCoord;
        }
    }

    if (printout_initialSchedule == 1)
    {
        cout << "MaxXCoord:" << MaxXCoord << ", MinXCoord" << MinXCoord << ", MaxYCoord:" << MaxYCoord << ", MinYCoord:" << MinYCoord << endl;
    }

    double NumberOfDivision = 4;

    int xDivision = ceil((MaxXCoord - MinXCoord) / NumberOfDivision);
    int yDivision = ceil((MaxYCoord - MinYCoord) / NumberOfDivision);
    vector<vector<int>> BlockCoord;
    if (printout_initialSchedule == 1)
    {
        cout << xDivision << "," << yDivision << endl;
    }
    int tempLeftXCoord = MinXCoord;
    int tempRightXCoord = MinXCoord;
    for (int i = 0; i < NumberOfDivision; i++)
    {
        tempLeftXCoord = tempRightXCoord;
        tempRightXCoord = tempRightXCoord + xDivision;

        int tempBottomYCoord = MinYCoord;
        int tempTopYCoord = MinYCoord;
        for (int j = 0; j < NumberOfDivision; j++)
        {
            tempBottomYCoord = tempTopYCoord;
            tempTopYCoord = tempTopYCoord + yDivision;
            vector<int> tempBlockCoord;
            tempBlockCoord.push_back(tempLeftXCoord);
            tempBlockCoord.push_back(tempRightXCoord);
            tempBlockCoord.push_back(tempBottomYCoord);
            tempBlockCoord.push_back(tempTopYCoord);
            BlockCoord.push_back(tempBlockCoord);
        }
    }
    if (printout_initialSchedule == 1)
    {
        for (int i = 0; i < BlockCoord.size(); i++)
        {
            cout << "LeftXCoord:" << BlockCoord[i][0] << ", RightXCoord:" << BlockCoord[i][1] << ", TopYCoord:" << BlockCoord[i][2] << ", BottomYCoord:" << BlockCoord[i][3] << endl;
        }
    }
    vector<vector<int>> SortedBlockCoord(BlockCoord);
    if (printout_initialSchedule == 1)
    {
        cout << "Unsorted blocks" << endl;
        for (int i = 0; i < SortedBlockCoord.size(); i++)
        {
            double DividByTwo = 2;
            double CenterXCoord = SortedBlockCoord[i][0] + ((SortedBlockCoord[i][1] - SortedBlockCoord[i][0]) / DividByTwo);
            double CenterYCoord = SortedBlockCoord[i][2] + ((SortedBlockCoord[i][3] - SortedBlockCoord[i][2]) / DividByTwo);

            double DistanceCenterToDepot = sqrt(pow(CenterXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterYCoord - IRPLR.Supplier.yCoord, power));
            cout << "CenterXCoord:" << CenterXCoord << " ,CenterYCoord:" << CenterYCoord << " ,DistanceCenterToDepot:" << DistanceCenterToDepot << endl;
        }
    }
    bool swap = true;
    while (swap == true)
    {
        swap = false;
        for (int i = 0; i < SortedBlockCoord.size() - 1; i++)
        {
            double DividByTwo = 2;
            double CenterOneXCoord = SortedBlockCoord[i][0] + ((SortedBlockCoord[i][1] - SortedBlockCoord[i][0]) / DividByTwo);
            double CenterOneYCoord = SortedBlockCoord[i][2] + ((SortedBlockCoord[i][3] - SortedBlockCoord[i][2]) / DividByTwo);

            double CenterTwoXCoord = SortedBlockCoord[i + 1][0] + ((SortedBlockCoord[i + 1][1] - SortedBlockCoord[i + 1][0]) / DividByTwo);
            double CenterTwoYCoord = SortedBlockCoord[i + 1][2] + ((SortedBlockCoord[i + 1][3] - SortedBlockCoord[i + 1][2]) / DividByTwo);

            double DistanceCenterOneToDepot = sqrt(pow(CenterOneXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterOneYCoord - IRPLR.Supplier.yCoord, power));
            double DistanceCenterTwoToDepot = sqrt(pow(CenterTwoXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterTwoYCoord - IRPLR.Supplier.yCoord, power));
            if (DistanceCenterOneToDepot < DistanceCenterTwoToDepot)
            {
                vector<int> tempSortedBlockCoord(SortedBlockCoord[i + 1]);
                SortedBlockCoord[i + 1] = SortedBlockCoord[i];
                SortedBlockCoord[i] = tempSortedBlockCoord;
                swap = true;
            }
        }
    }
    if (printout_initialSchedule == 1)
    {
        cout << "Sorted blocks" << endl;
        for (int i = 0; i < SortedBlockCoord.size(); i++)
        {
            double DividByTwo = 2;
            double CenterXCoord = SortedBlockCoord[i][0] + ((SortedBlockCoord[i][1] - SortedBlockCoord[i][0]) / DividByTwo);
            double CenterYCoord = SortedBlockCoord[i][2] + ((SortedBlockCoord[i][3] - SortedBlockCoord[i][2]) / DividByTwo);

            double DistanceCenterToDepot = sqrt(pow(CenterXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterYCoord - IRPLR.Supplier.yCoord, power));
            cout << "CenterXCoord:" << CenterXCoord << " ,CenterYCoord:" << CenterYCoord << " ,DistanceCenterToDepot:" << DistanceCenterToDepot << endl;
        }
    }

    vector<vector<vector<int>>> CompleteGroupOfRetailers;
    for (int i = 0; i < GroupOfRetailers.size(); i++) // For each stock out periods
    {
        vector<vector<int>> TempCompleteGroupOfRetailers;
        for (int x = 0; x < SortedBlockCoord.size(); x++)
        {
            vector<int> TempCompleteGroupOfRetailersForEachPeriod;
            TempCompleteGroupOfRetailers.push_back(TempCompleteGroupOfRetailersForEachPeriod);
        }

        for (int j = 0; j < GroupOfRetailers[i].size(); j++)
        {
            for (int k = 0; k < SortedBlockCoord.size(); k++)
            {
                if (IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord >= SortedBlockCoord[k][0] && IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord <= SortedBlockCoord[k][1] && IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord >= SortedBlockCoord[k][2] && IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord <= SortedBlockCoord[k][3])
                {
                    TempCompleteGroupOfRetailers[k].push_back(GroupOfRetailers[i][j]);
                    break;
                }
            }
        }
        CompleteGroupOfRetailers.push_back(TempCompleteGroupOfRetailers);
    }
    cout << "Sorting by demand" << endl;
    for (int i = 0; i < CompleteGroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < CompleteGroupOfRetailers[i].size(); j++)
        {
            if (CompleteGroupOfRetailers[i][j].size() != 0)
            {
                bool swap = true;
                while (swap == true)
                {
                    swap = false;
                    for (int k = 0; k < CompleteGroupOfRetailers[i][j].size() - 1; k++)
                    {
                        if (IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].Demand < IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k + 1]].Demand)
                        {
                            int tempRetailer = CompleteGroupOfRetailers[i][j][k];
                            CompleteGroupOfRetailers[i][j][k] = CompleteGroupOfRetailers[i][j][k + 1];
                            CompleteGroupOfRetailers[i][j][k + 1] = tempRetailer;
                            swap = true;
                        }
                    }
                }
            }
        }
    }

    cout << "Finished Sorting by demand" << endl;
    int counterNumberOfRetailers = 0;
    for (int i = 0; i < CompleteGroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < CompleteGroupOfRetailers[i].size(); j++)
        {
            if (printout_initialSchedule == 1)
            {
                cout << "Zone " << j << endl;
            }
            for (int k = 0; k < CompleteGroupOfRetailers[i][j].size(); k++)
            {
                int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, CompleteGroupOfRetailers[i][j][k]);
                double ratio_capacity_vs_demand = IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].InventoryMax / IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].Demand;
                if (printout_initialSchedule == 1)
                {
                    double DividByTwo = 2;
                    double CenterXCoord = SortedBlockCoord[j][0] + ((SortedBlockCoord[j][1] - SortedBlockCoord[j][0]) / DividByTwo);
                    double CenterYCoord = SortedBlockCoord[j][2] + ((SortedBlockCoord[j][3] - SortedBlockCoord[j][2]) / DividByTwo);
                    double DistanceCenterToDepot = sqrt(pow(CenterXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterYCoord - IRPLR.Supplier.yCoord, power));

                    cout << "Retailer " << CompleteGroupOfRetailers[i][j][k] << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << " ,MaxInventory:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].InventoryMax << " ,Demand:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].Demand << endl;
                    cout << "xCoord:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].xCoord << ", yCoord:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].yCoord << endl;
                    cout << "LeftXCoord:" << SortedBlockCoord[j][0] << ", RightXCoord:" << SortedBlockCoord[j][1] << ", TopYCoord:" << SortedBlockCoord[j][2] << ", BottomYCoord:" << SortedBlockCoord[j][3] << endl;
                    cout << "CenterXCoord:" << CenterXCoord << " ,CenterYCoord:" << CenterYCoord << " ,DistanceCenterToDepot:" << DistanceCenterToDepot << endl;
                }
                counterNumberOfRetailers++;
            }
        }
    }

    cout << IRPLR.Retailers.size() << "," << counterNumberOfRetailers << endl;
    assert(IRPLR.Retailers.size() == counterNumberOfRetailers);
    for (int i = 0; i < CompleteGroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < CompleteGroupOfRetailers[i].size(); j++)
        {
            vector<int> CandidateRetailers(CompleteGroupOfRetailers[i][j]);
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

                                if (printout_initialSchedule == 1)
                                {
                                    IRPSolution.print_solution(IRPLR);
                                }
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
                            if (printout_initialSchedule == 1)
                            {
                                IRPSolution.print_solution(IRPLR);
                            }
                        }
                    }
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