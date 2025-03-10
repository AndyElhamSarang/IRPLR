#include "lib.h"
void solution::ReadCVRP_Solution(input &IRPLR, int &day, vector<vector<int>> &RouteForDay)
{
    string RoutingFile = MachineDirectory + "Curtin/IRPLR/HGS-CVRP-main/Instances/IRP/IRPSolution.sol";
    vector<int> CustomerTracking;
    for (int j = 0; j < RouteForDay.size(); j++)
    {
        for (int k = 0; k < RouteForDay[j].size(); k++)
        {
            CustomerTracking.push_back(RouteForDay[j][k]);
        }
    }

    // cout << "Input routing" << endl;
    if (printout_initialReadCVRP == 1)
    {
        cout << "Customer Tracking" << endl;
        for (int i = 0; i < CustomerTracking.size(); i++)
        {
            cout << CustomerTracking[i] << ",";
        }
        cout << endl;
    }
    double TransportationCost = 0;
    string takeline;
    char taketab;
    ifstream ifroute;
    ifroute.open(RoutingFile.c_str());
    if (!ifroute.is_open())
    {
        exit(EXIT_FAILURE);
    };

    vector<vector<int>> OptimizedRoute;
    string TakeDummy;
    while (getline(ifroute, takeline))
    {
        stringstream ss_TakeLine(takeline);
        ss_TakeLine >> TakeDummy;
        // cout << "TakeDummy:" << TakeDummy << endl;
        // cout << takeline << endl;
        if (TakeDummy == "Route")
        {
            char TakeDummyChar;
            int TakeDummyInteger;
            ss_TakeLine >> TakeDummyChar >> TakeDummyInteger >> TakeDummyChar;
            vector<int> TempRoute;
            int TempVisit = IRPLR.Retailers.size();
            while (ss_TakeLine >> TempVisit)
            {
                TempVisit = CustomerTracking[TempVisit - 1];
                TempRoute.push_back(TempVisit);
                if (ss_TakeLine.peek() == ' ')
                {
                    ss_TakeLine.ignore();
                }
            }
            OptimizedRoute.push_back(TempRoute);
        }
        else if (TakeDummy == "Cost")
        {
            while (ss_TakeLine >> TransportationCost)
            {
                if (ss_TakeLine.peek() == ' ')
                {
                    ss_TakeLine.ignore();
                }
            }
        }
    }
    if (printout_initialReadCVRP == 1)
    {
        cout << "TransportationCost:" << TransportationCost << endl;
        cout << "OptimizedRoute:" << endl;
        for (int i = 0; i < OptimizedRoute.size(); i++)
        {
            for (int j = 0; j < OptimizedRoute[i].size(); j++)
            {
                cout << OptimizedRoute[i][j] << ",";
            }
            cout << endl;
        }
    }

    Route[day].clear();

    if (printout_initialReadCVRP == 1)
    {
        cout << "Remove unoptimized routes" << endl;
        for (int i = 0; i < Route.size(); i++)
        {
            cout << "Day " << i << endl;
            for (int j = 0; j < Route[i].size(); j++)
            {
                cout << "Vehicle " << j << ":";
                for (int k = 0; k < Route[i][j].size(); k++)
                {
                    cout << Route[i][j][k] << ",";
                }
                cout << endl;
            }
        }
    }
    for (int i = 0; i < OptimizedRoute.size(); i++)
    {
        Route[day].push_back(OptimizedRoute[i]);
        
    }
    while (Route[day].size() < IRPLR.NumberOfVehicles)
    {
        vector<int> EmptyRoute;
        Route[day].push_back(EmptyRoute);
    }

    if (printout_initialReadCVRP == 1)
    {
        cout << "Update with the optimized routes" << endl;
        for (int i = 0; i < Route.size(); i++)
        {
            cout << "Day " << i << endl;
            for (int j = 0; j < Route[i].size(); j++)
            {
                cout << "Vehicle " << j << ":";
                for (int k = 0; k < Route[i][j].size(); k++)
                {
                    cout << Route[i][j][k] << ",";
                }
                cout << endl;
            }
        }
    }
    for (int i = 0; i < Route[day].size(); i++)
    {
        double NewLoad = 0;
        for (int j = 0; j < Route[day][i].size(); j++)
        {
            NewLoad += DeliveryQuantity[Route[day][i][j]][day];
        }
        VehicleLoad[day][i] = NewLoad;
    }

    
}