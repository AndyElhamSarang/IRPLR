#include "lib.h"
void solution_construction::Initial_CircleZone_Schedule(input &IRPLR, solution &IRPSolution)
{
    if (printout_initialSchedule == 1)
    {
        cout << "INITIAL for IRPLR: multi vehicle case" << endl;
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
                cout << "Retailer " << GroupOfRetailers[i][j] << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand <<", xCoord:"<<IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord<<", yCoord:"<< IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord<<endl;
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

    int NumberOfCluster = 4;

    int LeftxLevel = (IRPLR.Supplier.xCoord - MinXCoord) / NumberOfCluster;
    int RightxLevel = (MaxXCoord - IRPLR.Supplier.xCoord) / NumberOfCluster;
    int LeftyLevel = (IRPLR.Supplier.yCoord - MinYCoord) / NumberOfCluster;
    int RightyLevel = (MaxYCoord - IRPLR.Supplier.yCoord) / NumberOfCluster;

    if (printout_initialSchedule == 1)
    {
        cout << "LeftxLevel:" << LeftxLevel << ", RightxLevel:" << RightxLevel << ", LeftyLevel:" << LeftyLevel << ", RightyLevel:" << RightyLevel << endl;
    }

    vector<vector<int>> ClusterRange;
    for (int i = 0; i < NumberOfCluster; i++)
    {
        vector<int> TempClusterRange;
        // 0 Left xCoord
        // 1 Right xCoord
        // 2 Left yCoord
        // 3 Right yCoord

        int LeftXCoord = 0;
        int RightXCoord = 0;
        int LeftYCoord = 0;
        int RightYCoord = 0;
        if (i == 0)
        {
            LeftXCoord = MinXCoord;
            RightXCoord = MaxXCoord;
            LeftYCoord = MinYCoord;
            RightYCoord = MaxYCoord;
        }
        else
        {
            LeftXCoord = ClusterRange[ClusterRange.size() - 1][0] + LeftxLevel;
            RightXCoord = ClusterRange[ClusterRange.size() - 1][1] - RightxLevel;
            LeftYCoord = ClusterRange[ClusterRange.size() - 1][2] + LeftyLevel;
            RightYCoord = ClusterRange[ClusterRange.size() - 1][3] - RightyLevel;
        }

        TempClusterRange.push_back(LeftXCoord);
        TempClusterRange.push_back(RightXCoord);
        TempClusterRange.push_back(LeftYCoord);
        TempClusterRange.push_back(RightYCoord);

        ClusterRange.push_back(TempClusterRange);
    }
    if (printout_initialSchedule == 1)
    {
        for (int i = 0; i < ClusterRange.size(); i++)
        {
            cout << "Left xCoord " << i + 1 << ":" << ClusterRange[i][0]
                 << ", Right xCoord " << i + 1 << ":" << ClusterRange[i][1]
                 << ", Left yCoord " << i + 1 << ":" << ClusterRange[i][2]
                 << ", Right yCoord " << i + 1 << ":" << ClusterRange[i][3] << endl;
        }
    }
    vector<vector<vector<int>>> CompleteGroupOfRetailers;
    for (int i = 0; i < GroupOfRetailers.size(); i++)//For each stock out periods
    {
        vector<vector<int>> TempCompleteGroupOfRetailers;
        for (int x = 0; x < ClusterRange.size(); x++)
        {
            vector<int> TempCompleteGroupOfRetailersForEachPeriod;
            TempCompleteGroupOfRetailers.push_back(TempCompleteGroupOfRetailersForEachPeriod);
        }

        for (int j = 0; j < GroupOfRetailers[i].size(); j++)
        {            
            for (int x = 0; x < ClusterRange.size(); x++)
            {
                if (x < ClusterRange.size() - 1)
                {
                    if (printout_initialSchedule == 1)
                    {
                        cout << ClusterRange[x][0] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << ClusterRange[x + 1][0] << "," << ClusterRange[x + 1][1] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << ClusterRange[x][1] << endl;
                        cout << ClusterRange[x][2] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << ClusterRange[x + 1][2] << "," << ClusterRange[x + 1][3] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << ClusterRange[x][3] << endl;
                    }
                    if ((ClusterRange[x][0] <= IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord < ClusterRange[x + 1][0]) || (ClusterRange[x + 1][1] < IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord <= ClusterRange[x][1]))
                    {
                        if ((ClusterRange[x][2] <= IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord &&  IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord <= ClusterRange[x][3]))
                        {
                            if (printout_initialSchedule == 1){cout << "!" << endl;}
                            TempCompleteGroupOfRetailers[x].push_back(GroupOfRetailers[i][j]);
                        }
                    }
                    if ((ClusterRange[x][2] <= IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord < ClusterRange[x + 1][2]) || (ClusterRange[x + 1][3] < IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord <= ClusterRange[x][3]))
                    {
                        if (ClusterRange[x][0] <= IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord <= ClusterRange[x][1])
                        {
                            int Check = 0;
                            for (int z = 0; z < TempCompleteGroupOfRetailers[x].size(); z++)
                            {
                                if (TempCompleteGroupOfRetailers[x][z] == GroupOfRetailers[i][j])
                                {
                                    Check = 1;
                                }
                            }
                            if (Check == 0)
                            {
                                if (printout_initialSchedule == 1){cout << "&" << endl;}
                                TempCompleteGroupOfRetailers[x].push_back(GroupOfRetailers[i][j]);
                            }
                        }
                    }
                }
                else
                {
                    if (printout_initialSchedule == 1)
                    {
                        cout << ClusterRange[x][0] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord << "," << ClusterRange[x][1] << endl;
                        cout << ClusterRange[x][2] << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord << "," << ClusterRange[x][3] << endl;
                    }
                    if (ClusterRange[x][0] <= IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].xCoord <= ClusterRange[x][1])
                    {
                        if (printout_initialSchedule == 1){cout<<"%"<<endl;}
                        if (ClusterRange[x][2] <= IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord && IRPLR.Retailers[GroupOfRetailers[i][j]].yCoord <= ClusterRange[x][3])
                        {
                            if (printout_initialSchedule == 1){cout<<"%"<<endl;}
                            TempCompleteGroupOfRetailers[x].push_back(GroupOfRetailers[i][j]);
                        }
                    }
                }
            }
        }
        CompleteGroupOfRetailers.push_back(TempCompleteGroupOfRetailers);
    }
    int counterNumberOfRetailers = 0;
    for (int i = 0; i < CompleteGroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < CompleteGroupOfRetailers[i].size(); j++)
        {
            if (printout_initialSchedule == 1){cout << "Zone " << j << endl;}
            for (int k = 0; k < CompleteGroupOfRetailers[i][j].size(); k++)
            {
                int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, CompleteGroupOfRetailers[i][j][k]);
                double ratio_capacity_vs_demand = IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].InventoryMax / IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].Demand;
                if (printout_initialSchedule == 1)
                {
                    cout << "Retailer " << CompleteGroupOfRetailers[i][j][k] << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << ", xCoord:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].xCoord << ", yCoord:" << IRPLR.Retailers[CompleteGroupOfRetailers[i][j][k]].yCoord << endl;
                }
                counterNumberOfRetailers++;
            }
        }
    }

    cout<<IRPLR.Retailers.size()<<","<< counterNumberOfRetailers<<endl;
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