#include "lib.h"

namespace
{
    bool ConstructAMMInitialSolution(input &IRPLR, solution &bestSolution)
    {
        const double epsilon = 1e-9;
        const int maxLookAhead = (IRPLR.TimeHorizon + 1) / 2;
        bool foundFeasibleSolution = false;
        double bestLogisticRatio = numeric_limits<double>::infinity();

        for (int ratioStep = 10; ratioStep > 0; --ratioStep)
        {
            const double demandRatio = ratioStep / 10.0;
            for (int lookAhead = 1; lookAhead <= maxLookAhead; ++lookAhead)
            {
                solution candidate;
                candidate.Initialization(IRPLR);
                vector<double> inventory(IRPLR.Retailers.size());
                for (int customer = 0; customer < static_cast<int>(IRPLR.Retailers.size()); ++customer)
                {
                    inventory[customer] = IRPLR.Retailers[customer].InventoryBegin;
                }

                double supplierInventory = IRPLR.Supplier.InventoryBegin;
                bool feasible = true;
                for (int day = 0; day < IRPLR.TimeHorizon && feasible; ++day)
                {
                    vector<pair<int, double>> mandatoryCustomers;
                    vector<pair<int, double>> candidateCustomers;
                    const bool isLastDay = day == IRPLR.TimeHorizon - 1;

                    for (int customer = 0; customer < static_cast<int>(IRPLR.Retailers.size()); ++customer)
                    {
                        const auto &retailer = IRPLR.Retailers[customer];
                        const double inventoryWithoutDelivery = inventory[customer] - retailer.Demand;
                        if (inventoryWithoutDelivery < -epsilon)
                        {
                            const double quantity = isLastDay
                                                        ? min(-inventoryWithoutDelivery, IRPLR.Vehicle.capacity)
                                                        : min(retailer.InventoryMax - inventory[customer],
                                                              IRPLR.Vehicle.capacity);
                            mandatoryCustomers.push_back({customer, max(0.0, quantity)});
                            continue;
                        }

                        bool withinLookAhead = false;
                        for (int offset = 1; offset <= lookAhead && day + offset < IRPLR.TimeHorizon; ++offset)
                        {
                            if (inventory[customer] - retailer.Demand * (offset + 1) < -epsilon)
                            {
                                withinLookAhead = true;
                                break;
                            }
                        }

                        const double quantity = min((retailer.InventoryMax - inventory[customer]) * demandRatio,
                                                    IRPLR.Vehicle.capacity);
                        // Criterion 2 of Alvarez et al. (2018): in the last period, visit the customer
                        // if holding the product at the supplier is more expensive than holding it at
                        // the customer plus a round trip. Kept as in the original, which uses the same
                        // construction heuristic for both objective functions.
                        const double estimatedRoundTripCost = 2.0 * IRPLR.Distance[0][customer + 1];
                        const bool economicalOnLastDay =
                            isLastDay &&
                            quantity * IRPLR.Supplier.InventoryCost >
                                quantity * retailer.InventoryCost + estimatedRoundTripCost;
                        if (quantity > epsilon && (withinLookAhead || economicalOnLastDay))
                        {
                            candidateCustomers.push_back({customer, quantity});
                        }
                    }

                    // Nearest-neighbour route construction (Alvarez et al., 2018, following
                    // Braysy and Gendreau, 2005). Vehicles are filled one after another: starting
                    // from the last node of the vehicle's route (the depot if empty), the nearest
                    // unrouted customer whose delivery fits the residual capacity is appended,
                    // until no such customer remains; then the next vehicle is used. Mandatory
                    // customers are routed first, then the optional customers extend the same
                    // routes. Deliveries are also limited by the supplier's available inventory.
                    double availableSupply = supplierInventory + IRPLR.Supplier.QuantityProduced;
                    const auto routeNearestNeighbour = [&](const vector<pair<int, double>> &customers) -> bool
                    {
                        vector<bool> routed(customers.size(), false);
                        size_t numberRouted = 0;
                        for (int vehicle = 0; vehicle < IRPLR.NumberOfVehicles && numberRouted < customers.size();
                             ++vehicle)
                        {
                            vector<int> &route = candidate.Route[day][vehicle];
                            int currentNode = route.empty() ? 0 : route.back() + 1;
                            while (numberRouted < customers.size())
                            {
                                int nearestIndex = -1;
                                double nearestDistance = numeric_limits<double>::infinity();
                                double nearestQuantity = 0.0;
                                for (int index = 0; index < static_cast<int>(customers.size()); ++index)
                                {
                                    if (routed[index])
                                    {
                                        continue;
                                    }
                                    const double quantity = min(customers[index].second, availableSupply);
                                    if (quantity <= epsilon ||
                                        candidate.VehicleLoad[day][vehicle] + quantity >
                                            IRPLR.Vehicle.capacity + epsilon)
                                    {
                                        continue;
                                    }
                                    const double distance =
                                        IRPLR.Distance[currentNode][customers[index].first + 1];
                                    if (distance < nearestDistance)
                                    {
                                        nearestDistance = distance;
                                        nearestIndex = index;
                                        nearestQuantity = quantity;
                                    }
                                }
                                if (nearestIndex < 0)
                                {
                                    break;
                                }

                                const int customer = customers[nearestIndex].first;
                                route.push_back(customer);
                                candidate.DeliveryQuantity[customer][day] = nearestQuantity;
                                candidate.VehicleLoad[day][vehicle] += nearestQuantity;
                                candidate.TotalDeliveryPerDay[day] += nearestQuantity;
                                availableSupply -= nearestQuantity;
                                inventory[customer] += nearestQuantity;
                                routed[nearestIndex] = true;
                                ++numberRouted;
                                currentNode = customer + 1;
                            }
                        }
                        return numberRouted == customers.size();
                    };

                    // All mandatory customers must be visited; otherwise this parameter
                    // combination yields no solution.
                    if (!routeNearestNeighbour(mandatoryCustomers))
                    {
                        feasible = false;
                        break;
                    }
                    // Optional customers that do not fit are simply not visited.
                    routeNearestNeighbour(candidateCustomers);

                    supplierInventory = availableSupply;
                    if (supplierInventory < -epsilon)
                    {
                        feasible = false;
                    }
                    for (int customer = 0; customer < static_cast<int>(IRPLR.Retailers.size()); ++customer)
                    {
                        inventory[customer] -= IRPLR.Retailers[customer].Demand;
                        candidate.InventoryLevel[customer][day] = inventory[customer];
                        if (inventory[customer] < -epsilon)
                        {
                            feasible = false;
                        }
                    }
                    candidate.InventoryLevelSupplier[day] = supplierInventory;
                }

                if (!feasible)
                {
                    continue;
                }

                candidate.UpdateVehicleAllocationVisitOrder(IRPLR);
                candidate.GetLogisticRatio(IRPLR);
                if (candidate.TotalDelivery <= epsilon)
                {
                    candidate.LogisticRatio = 0.0;
                }
                if (!foundFeasibleSolution || candidate.LogisticRatio < bestLogisticRatio)
                {
                    bestSolution = candidate;
                    bestLogisticRatio = candidate.LogisticRatio;
                    foundFeasibleSolution = true;
                }
            }
        }

        return foundFeasibleSolution;
    }

    void ConstructNearestInsertionRoutes(input &IRPLR, solution &IRPSolution)
    {
        bool flag = false;
        for (int day = 0; day < static_cast<int>(IRPSolution.Route.size()); ++day)
        {
            for (int vehicle = 0; vehicle < static_cast<int>(IRPSolution.Route[day].size()); ++vehicle)
            {
                vector<int> unvisited(IRPSolution.Route[day][vehicle]);
                vector<int> route;
                if (unvisited.empty())
                {
                    continue;
                }

                // Seed the tour with the customer nearest to the depot.
                int seedIndex = 0;
                for (int i = 0; i < static_cast<int>(unvisited.size()); ++i)
                {
                    if (IRPLR.Distance[0][unvisited[i] + 1] < IRPLR.Distance[0][unvisited[seedIndex] + 1])
                    {
                        seedIndex = i;
                    }
                }
                route.push_back(unvisited[seedIndex]);
                unvisited.erase(unvisited.begin() + seedIndex);

                // Repeatedly select the unvisited customer nearest to the current
                // closed tour, then insert it where it adds the least distance.
                while (!unvisited.empty())
                {
                    int nearestIndex = 0;
                    double nearestDistance = numeric_limits<double>::infinity();
                    for (int i = 0; i < static_cast<int>(unvisited.size()); ++i)
                    {
                        double distanceToTour = IRPLR.Distance[0][unvisited[i] + 1];
                        for (int customer : route)
                        {
                            distanceToTour = min(distanceToTour, static_cast<double>(
                                                                     IRPLR.Distance[customer + 1][unvisited[i] + 1]));
                        }
                        if (distanceToTour < nearestDistance)
                        {
                            nearestDistance = distanceToTour;
                            nearestIndex = i;
                        }
                    }

                    const int customer = unvisited[nearestIndex];
                    int bestPosition = 0;
                    double bestIncrease = numeric_limits<double>::infinity();
                    for (int position = 0; position <= static_cast<int>(route.size()); ++position)
                    {
                        const int previousNode = (position == 0) ? 0 : route[position - 1] + 1;
                        const int nextNode = (position == static_cast<int>(route.size()))
                                                 ? 0
                                                 : route[position] + 1;
                        const double previousToCustomer = IRPLR.Distance[previousNode][customer + 1];
                        const double customerToNext = IRPLR.Distance[customer + 1][nextNode];
                        const double removedEdge = IRPLR.Distance[previousNode][nextNode];
                        const double increase = previousToCustomer + customerToNext - removedEdge;
                        if (increase < bestIncrease)
                        {
                            bestIncrease = increase;
                            bestPosition = position;
                        }
                    }

                    route.insert(route.begin() + bestPosition, customer);
                    unvisited.erase(unvisited.begin() + nearestIndex);
                }

                IRPSolution.Route[day][vehicle].swap(route);
                for (int position = 0; position < static_cast<int>(IRPSolution.Route[day][vehicle].size()); ++position)
                {
                    const int customer = IRPSolution.Route[day][vehicle][position];
                    IRPSolution.VehicleAllocation[customer][day] = vehicle;
                    IRPSolution.VisitOrder[customer][day] = position;
                }
            }
        }
    }
} // namespace

void solution_construction::INITIAL_ZONE(input &IRPLR, solution &IRPSolution, HGS &Routing, solution &GlobalBest, file &read_file, int &MS_ITERATION)
{
    if (printout_initial == 1)
    {
        cout << "Construct initial solution using INITIAL ZONE" << endl;
    }
    if (InitialSolutionMethod == "ConstructAMMInitialSolution")
    {
        if (!ConstructAMMInitialSolution(IRPLR, IRPSolution))
        {
            throw runtime_error("AMM initial-solution construction could not find a feasible solution");
        }
        time(&end_time);
        double TimeForInitialAMM = difftime(end_time, start_time);
        IRPSolution.GetLogisticRatio(IRPLR);
        if (printout_initial == 1)
        {
            cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
        }
        if (OutputResults == 1)
        {
            Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << "," << TimeForInitialAMM << ",";
        }

        if (printout_initial == 1)
        {
            cout << "Initial solution after AMM initial procedure" << endl;
            IRPSolution.print_solution(IRPLR);
        }
        if (OutputSolutionJSON == "YES")
        {
            IRPSolution.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_initial_solution_from_AMM_" + to_string(MS_ITERATION) + ".json");
        }
    }
    else if (InitialSolutionMethod == "Initial_BlockZone_Schedule")
    {

        // assert(GridResolution > 0);
        // Initial_CircleZone_Schedule(IRPLR, IRPSolution);
        Initial_BlockZone_Schedule(IRPLR, IRPSolution);
        time(&end_time);
        double TimeForScheduling = difftime(end_time, start_time);
        IRPSolution.GetLogisticRatio(IRPLR);
        if (printout_initial == 1)
        {
            cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
        }
        if (OutputResults == 1)
        {
            Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << "," << TimeForScheduling << ",";
        }

        if (printout_initial == 1)
        {
            cout << "Initial solution" << endl;
            IRPSolution.print_solution(IRPLR);
        }
        if (OutputSolutionJSON == "YES")
        {
            IRPSolution.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_initial_solution_" + to_string(MS_ITERATION) + ".json");
        }
        ///////////////////////////////////////////////
        //                                           //
        //              Nearest insertion            //
        //                                           //
        ///////////////////////////////////////////////
        if (printout_initial == 1)
        {
            cout << "Nearest insertion..." << endl;
        }
        ConstructNearestInsertionRoutes(IRPLR, IRPSolution);
        time(&end_time);
        double TimeForNearestInsertion = difftime(end_time, start_time);
        IRPSolution.GetLogisticRatio(IRPLR);
        if (printout_initial == 1)
        {
            cout << "TotalTransportationCostAfterNearestInsertion:" << IRPSolution.TotalTransportationCost << "\t TotalDeliveryAfterNearestInsertion:" << IRPSolution.TotalDelivery << "\t LogistcRatioAfterNearestInsertion:" << IRPSolution.LogisticRatio << endl;
        }
        if (OutputResults == 1)
        {
            Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << "," << TimeForNearestInsertion << ",";
        }

        if (printout_initial == 1)
        {
            cout << "Initial solution after nearest insertion" << endl;
            IRPSolution.print_solution(IRPLR);
        }
        if (OutputSolutionJSON == "YES")
        {
            IRPSolution.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_initial_solution_after_nearest_insertion_" + to_string(MS_ITERATION) + ".json");
        }
    }
    else
    {
        throw runtime_error("InitialSolutionMethod must be ConstructAMMInitialSolution or Initial_BlockZone_Schedule");
    }

    ///////////////////////////////////////////////
    //                                           //
    //               HGS Routing                 //
    //                                           //
    ///////////////////////////////////////////////
    time(&HGS_start_time);
    if (ActivateHGS == "YES")
    {
        for (int i = 0; i < IRPSolution.Route.size(); i++)
        {
            int NumberOfCustomerOfDay = 0;
            for (int j = 0; j < IRPSolution.Route[i].size(); j++)
            {
                NumberOfCustomerOfDay += IRPSolution.Route[i][j].size();
            }
            if (NumberOfCustomerOfDay > 1)
            {
                IRPSolution.OutputCVRP(IRPLR, i, IRPSolution.Route[i]);
                Routing.CallHGS(IRPLR);
                IRPSolution.ReadCVRP_Solution(IRPLR, i, IRPSolution.Route[i]);
            }
        }
        IRPSolution.GetLogisticRatio(IRPLR);
        if (printout_initial == 1)
        {
            cout << "Solution after Optimizing the routes" << endl;
            IRPSolution.print_solution(IRPLR);
            cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
        }
        if (OutputSolutionJSON == "YES")
        {
            IRPSolution.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_initial_afterHGS_solution_" + to_string(MS_ITERATION) + ".json");
        }
    }
    time(&HGS_end_time);
    double TimeForHGS = difftime(HGS_end_time, HGS_start_time);
    AccumulatedTimeHGS += TimeForHGS;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (OutputResults == 1)
    {
        Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << "," << TimeForHGS << ",";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cout << "IRPLR.NumberOfRetailers:" << IRPLR.NumberOfRetailers << ", " << "IRPLR.Retailers.size():" << IRPLR.Retailers.size() << endl;
    for (int i = 0; i < IRPSolution.VisitOrder.size(); i++)
    {
        for (int j = 0; j < IRPSolution.VisitOrder[i].size(); j++)
        {
            IRPSolution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
            IRPSolution.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
        }
    }
    IRPSolution.UnallocatedCustomers.clear();
    for (int i = 0; i < IRPSolution.Route.size(); i++) // For this time period
    {
        vector<int> TempUnallocatedCustomer;             // Look for unallcated customers at this time period
        for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
        {
            int UnallocatedYesOrNo = 0;
            for (int j = 0; j < IRPSolution.Route[i].size(); j++) // index j for vehicle
            {
                for (int k = 0; k < IRPSolution.Route[i][j].size(); k++) // index k for position
                {

                    if (IRPSolution.Route[i][j][k] == x)
                    {
                        UnallocatedYesOrNo = 1;
                        IRPSolution.VehicleAllocation[x][i] = j;
                        IRPSolution.VisitOrder[x][i] = k;
                    }
                }
            }
            if (UnallocatedYesOrNo == 0) // This customer is not visited
            {
                TempUnallocatedCustomer.push_back(x);
            }
        }
        IRPSolution.UnallocatedCustomers.push_back(TempUnallocatedCustomer);
    }
    if (GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
    {
        GlobalBest = IRPSolution;
    }
}