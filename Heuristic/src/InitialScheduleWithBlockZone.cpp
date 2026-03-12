#include "lib.h"

/*
 This function constructs an initial delivery schedule using a block/zone partitioning
 of the geographic area. It assigns retailers to delivery periods and vehicles
 such that stock-outs are avoided where possible and vehicle capacity constraints
 are respected.

 High-level steps:
  - Initialize the solution structure.
  - Group retailers by their next stock-out period.
  - Partition the map into rectangular blocks (zones).
  - Sort blocks by distance of their centers to the depot.
  - For each stock-out period and block, sort retailers by demand (descending).
  - Iteratively assign retailers (random selection within zone) to feasible periods
    and vehicles, updating delivery quantities, vehicle loads and retailer inventories.
*/

void solution_construction::Initial_BlockZone_Schedule(input &IRPLR, solution &IRPSolution)
{
    // Print basic info when debugging is enabled
    if (printout_initialSchedule == 1)
    {
        cout << "INITIAL for IRPLR: multi vehicle case" << endl;
        cout << "Create schedule with block type zone" << endl;
        cout << "Vehicle capacity:" << IRPLR.Vehicle.capacity << endl;
        cout << "Vehicles total capacity:" << IRPLR.VehiclesTotalCapacity << endl;
    }

    // Initialize solution arrays and structures
    IRPSolution.Initialization(IRPLR);
    
    if (printout_initialSchedule == 1)
    {
        IRPSolution.print_solution(IRPLR);
    }

    boost_random_mechanism RandomInitial; // random helper for initial schedule

    // GroupOfRetailers[t] will contain retailer indices whose next stock-out is period t
    vector<vector<int>> GroupOfRetailers;

    // Initialize groups for each period in the horizon
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<int> TempGroupOfRetailers;
        GroupOfRetailers.push_back(TempGroupOfRetailers);
    }

    // Assign each retailer to the group corresponding to its next stock-out period
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        int checkNextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, i);
        double ratio_capacity_vs_demand = IRPLR.Retailers[i].InventoryMax / IRPLR.Retailers[i].Demand;
        if (printout_initialSchedule == 1)
        {
            cout << "Retailer " << i << ": checkNextStockOutPeriod:" << checkNextStockOutPeriod << ", ratio_capacity_vs_demand:" << ratio_capacity_vs_demand << endl;
        }
        // Add retailer index to its stock-out period group
        GroupOfRetailers[checkNextStockOutPeriod].push_back(i);
    }
    if (printout_initialSchedule == 1)
    {
        cout << "Sorted via stock out period" << endl;
    }

    // Sanity counting of grouped retailers for debug/assertion
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

    // Determine bounding box of all coordinates (supplier + retailers)
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

    // Number of divisions per dimension for block partitioning (e.g., 4x4 grid)
    double NumberOfDivision = 4;

    // Compute block size in x and y (ceil to ensure coverage)
    int xDivision = ceil((MaxXCoord - MinXCoord) / NumberOfDivision);
    int yDivision = ceil((MaxYCoord - MinYCoord) / NumberOfDivision);
    vector<vector<int>> BlockCoord; // Each entry: {LeftX, RightX, BottomY, TopY}

    if (printout_initialSchedule == 1)
    {
        cout << xDivision << "," << yDivision << endl;
    }

    // Build block coordinates by iterating grid cells
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
            // store block bounding box: left, right, bottom, top
            tempBlockCoord.push_back(tempLeftXCoord);
            tempBlockCoord.push_back(tempRightXCoord);
            tempBlockCoord.push_back(tempBottomYCoord);
            tempBlockCoord.push_back(tempTopYCoord);
            BlockCoord.push_back(tempBlockCoord);
        }
    }

    // Debug print of generated blocks
    if (printout_initialSchedule == 1)
    {
        for (int i = 0; i < BlockCoord.size(); i++)
        {
            cout << "LeftXCoord:" << BlockCoord[i][0] << ", RightXCoord:" << BlockCoord[i][1] << ", TopYCoord:" << BlockCoord[i][2] << ", BottomYCoord:" << BlockCoord[i][3] << endl;
        }
    }

    // Make a copy for sorting by distance to depot
    vector<vector<int>> SortedBlockCoord(BlockCoord);

    if (printout_initialSchedule == 1)
    {
        cout << "Unsorted blocks" << endl;
        for (int i = 0; i < SortedBlockCoord.size(); i++)
        {
            double DividByTwo = 2;
            // center coordinates of the block (midpoint)
            double CenterXCoord = SortedBlockCoord[i][0] + ((SortedBlockCoord[i][1] - SortedBlockCoord[i][0]) / DividByTwo);
            double CenterYCoord = SortedBlockCoord[i][2] + ((SortedBlockCoord[i][3] - SortedBlockCoord[i][2]) / DividByTwo);

            double DistanceCenterToDepot = sqrt(pow(CenterXCoord - IRPLR.Supplier.xCoord, power) + pow(CenterYCoord - IRPLR.Supplier.yCoord, power));
            cout << "CenterXCoord:" << CenterXCoord << " ,CenterYCoord:" << CenterYCoord << " ,DistanceCenterToDepot:" << DistanceCenterToDepot << endl;
        }
    }

    // Bubble-sort blocks by distance to depot (descending order: farthest first)
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
            // swap if center one is closer than center two -> produce descending order
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

    // CompleteGroupOfRetailers[period][block] -> list of retailers in that period assigned to that block
    vector<vector<vector<int>>> CompleteGroupOfRetailers;
    for (int i = 0; i < GroupOfRetailers.size(); i++) // For each stock out period
    {
        vector<vector<int>> TempCompleteGroupOfRetailers;
        // create empty list for each block
        for (int x = 0; x < SortedBlockCoord.size(); x++)
        {
            vector<int> TempCompleteGroupOfRetailersForEachPeriod;
            TempCompleteGroupOfRetailers.push_back(TempCompleteGroupOfRetailersForEachPeriod);
        }

        // assign retailers in this period to one of the blocks based on coordinates
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

    // Sort retailers within each block by demand (descending) to prioritize high demand retailers first
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

    // Verify every retailer is present in the complete grouping
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

    // Main assignment loop:
    // For each period and block, maintain a list of candidate retailers and repeatedly pick one at random,
    // then find a feasible period (forward/backward search) to serve it, allocate to a vehicle and update loads/inventories.
    for (int i = 0; i < CompleteGroupOfRetailers.size(); i++)
    {
        for (int j = 0; j < CompleteGroupOfRetailers[i].size(); j++)
        {
            // Copy list of candidate retailers for this (period, block)
            vector<int> CandidateRetailers(CompleteGroupOfRetailers[i][j]);
            while (CandidateRetailers.size() != 0)
            {
                // Randomly pick a retailer from candidates
                int RandomPickARetailer = RandomInitial.random_number_generator(0, CandidateRetailers.size() - 1, generator);

                // The following variables control the search for an appropriate delivery period
                int StockOutExist = 0;
                int CurrentPeriod = 0;
                int NextStockOutPeriod = 0;

                // Loop until we assign this retailer or determine it has no future stock-out (then remove)
                while (NextStockOutPeriod < IRPLR.TimeHorizon + 1)
                {
                    NextStockOutPeriod = IRPSolution.CheckStockOut(IRPLR, CandidateRetailers[RandomPickARetailer]);
                    if (printout_initialSchedule == 1)
                    {
                        cout << "NextStockOutPeriod:" << NextStockOutPeriod << endl;
                    }

                    if (NextStockOutPeriod >= IRPLR.TimeHorizon + 1)
                    {
                        // No further stock out -> retailer needs no delivery, remove from candidates
                        CandidateRetailers.erase(CandidateRetailers.begin() + RandomPickARetailer, CandidateRetailers.begin() + RandomPickARetailer + 1);
                        CurrentPeriod = IRPLR.TimeHorizon + 1;
                        break;
                    }
                    else
                    {
                        // If current period is before or equal to the next stock-out, try to schedule forward into [CurrentPeriod..NextStockOutPeriod]
                        if (CurrentPeriod <= NextStockOutPeriod)
                        {
                            if (printout_initialSchedule == 1)
                            {
                                cout << "going forward" << endl;
                                cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;
                            }

                            // Collect potential periods where this retailer is not already visited and there is remaining vehicle capacity
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
                                            VisitedOrNot = 1; // already assigned in that period
                                    }
                                    TotalLoad += IRPSolution.VehicleLoad[i][j]; // sum vehicle loads in period i
                                }
                                // If total load equals total fleet capacity consider period full
                                if (fabs(TotalLoad - IRPLR.Vehicle.capacity * IRPLR.NumberOfVehicles) < 0.00001)
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
                                // No feasible forward period found; advance current period beyond next stock-out and try again (search further)
                                if (printout_initialSchedule == 1)
                                {
                                    cout << "No Potential periods available, going backward further" << endl;
                                }
                                CurrentPeriod = NextStockOutPeriod + 1;
                            }
                            else
                            {
                                // Randomly select one feasible period from potentials
                                int RandomPotentialPeirod = RandomInitial.random_number_generator(0, PotentialPeriods.size() - 1, generator);
                                int RandomPickANonStockOutPeriod = PotentialPeriods[RandomPotentialPeirod];
                                if (printout_initialSchedule == 1)
                                {
                                    cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << ", SelectedTimePeriod: " << RandomPickANonStockOutPeriod << endl;
                                }

                                // Determine vehicle and delivery quantity for the selected period
                                if (RandomPickANonStockOutPeriod > 0)
                                {
                                    double Load = 0;
                                    int vehicle_index = IRPLR.NumberOfVehicles; // sentinel; gets set to chosen vehicle index
                                    // Try to find a vehicle with room to deliver up to retailer's capacity (InventoryMax limited by remaining inventory level)
                                    for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
                                    {
                                        Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod - 1]);
                                        if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] < Load)
                                        {
                                            IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = Load;
                                            vehicle_index = i;
                                        }
                                    }
                                    
                                    // Assign retailer to the found vehicle's route for that period and update load/allocation/order
                                    IRPSolution.Route[RandomPickANonStockOutPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
                                    IRPSolution.VehicleLoad[RandomPickANonStockOutPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod];
                                    IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = vehicle_index;
                                    IRPSolution.VisitOrder[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = IRPSolution.Route[RandomPickANonStockOutPeriod][vehicle_index].size()-1;
                                }
                                else
                                {
                                    // Special handling for period 0: inventory baseline is InventoryBegin
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
                                    IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = vehicle_index;
                                    IRPSolution.VisitOrder[CandidateRetailers[RandomPickARetailer]][RandomPickANonStockOutPeriod] = IRPSolution.Route[RandomPickANonStockOutPeriod][vehicle_index].size()-1;
                                }

                                // After assigning a delivery, update the inventory level timeline for the retailer
                                int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
                                for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
                                {
                                    tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
                                    IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
                                }
                                // Move current period forward after assignment
                                CurrentPeriod = RandomPickANonStockOutPeriod + 1;

                                if (printout_initialSchedule == 1)
                                {
                                    IRPSolution.print_solution(IRPLR);
                                }
                            }
                        }
                        else
                        {
                            // If CurrentPeriod > NextStockOutPeriod we must search backwards to find an earlier feasible period
                            if (printout_initialSchedule == 1)
                            {
                                cout << "Going backward" << endl;
                                cout << "Period for Retailer " << CandidateRetailers[RandomPickARetailer] << ", CurrentPeriod: " << CurrentPeriod << ", NextStockOutPeriod: " << NextStockOutPeriod << endl;
                            }

                            // LookBackwardPeriod will hold the selected earlier period where we can deliver
                            int LookBackwardPeriod = IRPLR.TimeHorizon + 1;

                            // Search backwards from NextStockOutPeriod-1 down to period 0
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
                                if (fabs(TotalLoad - IRPLR.Vehicle.capacity * IRPLR.NumberOfVehicles) < 0.00001)
                                    VisitedOrNot = 1;
                                if (VisitedOrNot == 0)
                                {
                                    // Found a feasible backward period
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

                            // Assign to a vehicle similarly as in forward search, using LookBackwardPeriod
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
                                IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
                                IRPSolution.VisitOrder[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = IRPSolution.Route[LookBackwardPeriod][vehicle_index].size()-1;
                            }
                            else
                            {
                                // Special handling for period 0 using InventoryBegin baseline
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
                                IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
                                IRPSolution.VisitOrder[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = IRPSolution.Route[LookBackwardPeriod][vehicle_index].size()-1; 
                            }

                            // Update inventory timeline after assignment
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

    // Final sanity checks on route dimensions
    assert(IRPSolution.Route.size() == IRPLR.TimeHorizon);
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        assert(IRPSolution.Route[i].size() == IRPLR.NumberOfVehicles);
    }
}