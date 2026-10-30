


#include "lib.h"

namespace
{
    struct DisturbanceVisit
    {
        int vehicle;
        int customer;
    };
}

void solution_improvement::OperatorDisturb_Shaw1998_Pisinger2007(
    input &IRPLR,
    solution &IncumbentSolution,
    solution &IRPSolution,
    int &DisturbanceCounter,
    int &MaxDisturbance)
{
    IRPSolution = IncumbentSolution;
    if (MaxDisturbance <= 0 || IRPLR.NumberOfVehicles <= 0 || IRPLR.TimeHorizon <= 0)
    {
        return;
    }

    boost_random_mechanism randomness;
    const long long scaledStrength = max(
        1LL,
        1LL + static_cast<long long>(DisturbStrength) * max(0, DisturbanceCounter) / MaxDisturbance);
    const int disturbanceLimit = IRPLR.NumberOfVehicles;
    const int disturbancesToApply =
        static_cast<int>(min(scaledStrength, static_cast<long long>(disturbanceLimit)));

    int disturbancesApplied = 0;
    while (disturbancesApplied < disturbancesToApply)
    {
        vector<int> eligibleDays;
        for (int day = 0; day < static_cast<int>(IRPSolution.Route.size()); ++day)
        {
            int visitsToday = 0;
            for (const vector<int> &route : IRPSolution.Route[day])
            {
                visitsToday += static_cast<int>(route.size());
            }
            if (visitsToday >= 2)
            {
                eligibleDays.push_back(day);
            }
        }
        if (eligibleDays.empty())
        {
            break;
        }

        int day = eligibleDays[randomness.random_number_generator(
            0, static_cast<int>(eligibleDays.size()) - 1, generator)];
        vector<int> eligibleVehicles;
        int totalVisits = 0;
        for (int vehicle = 0; vehicle < static_cast<int>(IRPSolution.Route[day].size()); ++vehicle)
        {
            const int routeSize = static_cast<int>(IRPSolution.Route[day][vehicle].size());
            if (routeSize > 0)
            {
                eligibleVehicles.push_back(vehicle);
                totalVisits += routeSize;
            }
        }

        const int seedVehicle = eligibleVehicles[randomness.random_number_generator(
            0, static_cast<int>(eligibleVehicles.size()) - 1, generator)];
        const vector<int> &seedRoute = IRPSolution.Route[day][seedVehicle];
        const int seedPosition = randomness.random_number_generator(
            0, static_cast<int>(seedRoute.size()) - 1, generator);
        const int maximumRemovalSize = max(2, static_cast<int>(seedRoute.size()));
        const int numberToRemove = randomness.random_number_generator(
            2, min(totalVisits, maximumRemovalSize), generator);

        vector<DisturbanceVisit> selectedVisits;
        selectedVisits.push_back({seedVehicle, seedRoute[seedPosition]});

        while (static_cast<int>(selectedVisits.size()) < numberToRemove)
        {
            vector<pair<int, DisturbanceVisit>> relatedVisits;
            for (int vehicle = 0; vehicle < static_cast<int>(IRPSolution.Route[day].size()); ++vehicle)
            {
                for (int customer : IRPSolution.Route[day][vehicle])
                {
                    const auto alreadySelected = find_if(
                        selectedVisits.begin(),
                        selectedVisits.end(),
                        [customer](const DisturbanceVisit &selected)
                        {
                            return selected.customer == customer;
                        });
                    if (alreadySelected == selectedVisits.end())
                    {
                        int closestDistance = numeric_limits<int>::max();
                        for (const DisturbanceVisit &selected : selectedVisits)
                        {
                            closestDistance = min(
                                closestDistance,
                                IRPLR.Distance[customer + 1][selected.customer + 1]);
                        }
                        relatedVisits.push_back({closestDistance, {vehicle, customer}});
                    }
                }
            }
            if (relatedVisits.empty())
            {
                break;
            }

            sort(
                relatedVisits.begin(),
                relatedVisits.end(),
                [](const pair<int, DisturbanceVisit> &left, const pair<int, DisturbanceVisit> &right)
                {
                    if (left.first != right.first)
                    {
                        return left.first < right.first;
                    }
                    return left.second.customer < right.second.customer;
                });

            const int randomCandidateCount = min(3, static_cast<int>(relatedVisits.size()));
            // Randomize within the closest candidates to diversify related removals.
            const int selectedIndex = randomness.random_number_generator(0, randomCandidateCount - 1, generator);
            selectedVisits.push_back(relatedVisits[selectedIndex].second);
        }

        for (const DisturbanceVisit &visit : selectedVisits)
        {
            int customer = visit.customer;
            int vehicle = visit.vehicle;
            const double removedQuantity = IRPSolution.DeliveryQuantity[customer][day];
            IRPSolution.VehicleLoad[day][vehicle] -= removedQuantity;
            IRPSolution.DeliveryQuantity[customer][day] = 0.0;
            IRPSolution.VehicleAllocation[customer][day] = IRPLR.NumberOfVehicles + 1;
            IRPSolution.VisitOrder[customer][day] = static_cast<int>(IRPLR.Retailers.size()) + 1;

            double previousInventory =
                day == 0 ? IRPLR.Retailers[customer].InventoryBegin
                         : IRPSolution.InventoryLevel[customer][day - 1];
            double changeInTotalQuantity = -removedQuantity;
            double newStockOut = 0.0;
            double newVehicleOverload = 0.0;
            AdjustQuantityAndInventoryLevel(
                previousInventory,
                day,
                vehicle,
                IRPSolution.DeliveryQuantity[customer],
                IRPSolution.InventoryLevel[customer],
                IRPSolution.VehicleLoad,
                IRPSolution.VehicleAllocation,
                changeInTotalQuantity,
                newStockOut,
                newVehicleOverload,
                customer,
                IRPLR);

            vector<int> &unallocated = IRPSolution.UnallocatedCustomers[day];
            if (find(unallocated.begin(), unallocated.end(), customer) == unallocated.end())
            {
                unallocated.push_back(customer);
            }
        }

        for (const DisturbanceVisit &visit : selectedVisits)
        {
            vector<int> &route = IRPSolution.Route[day][visit.vehicle];
            route.erase(remove(route.begin(), route.end(), visit.customer), route.end());
        }
        IRPSolution.UpdateVehicleAllocationVisitOrder(IRPLR);

        disturbancesApplied++;
    }

    IRPSolution.StockoutSupplier = 0.0;
    double supplierInventory = IRPLR.Supplier.InventoryBegin;
    for (int day = 0; day < IRPLR.TimeHorizon; ++day)
    {
        double totalDelivery = 0.0;
        for (const vector<double> &customerDelivery : IRPSolution.DeliveryQuantity)
        {
            totalDelivery += customerDelivery[day];
        }
        IRPSolution.TotalDeliveryPerDay[day] = totalDelivery;
        supplierInventory += IRPLR.Supplier.QuantityProduced - totalDelivery;
        IRPSolution.InventoryLevelSupplier[day] = supplierInventory;
        IRPSolution.StockoutSupplier += max(0.0, -supplierInventory);
    }
}
