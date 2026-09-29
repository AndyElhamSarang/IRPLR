#include "lib.h"

namespace
{
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

void solution_construction::INITIAL_ZONE(input &IRPLR, solution &IRPSolution, HGS &Routing,solution &GlobalBest, file &read_file, int &MS_ITERATION)
{
    if (printout_initial == 1)
    {
        cout << "Construct initial solution using INITIAL ZONE" << endl;
    }
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
        Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << ","<< TimeForHGS << ",";
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
            for (int j = 0; j < IRPSolution.Route[i].size(); j++)// index j for vehicle
            {
                for (int k = 0; k < IRPSolution.Route[i][j].size(); k++) //index k for position
                {

                    if (IRPSolution.Route[i][j][k] == x)
                    {
                        UnallocatedYesOrNo = 1;
                        IRPSolution.VehicleAllocation[x][i] = j;
                        IRPSolution.VisitOrder [x][i] = k;
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
    if(GlobalBest.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
    {
        GlobalBest=IRPSolution;
    }

}