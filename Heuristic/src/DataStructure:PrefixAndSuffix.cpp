#include "lib.h"
void preprocessing::PopulatePrefixAndSuffix(input &IRPLR, solution &IRPSolution)
{
    //cout << "PopulateLocalPrefixAndSuffix" << endl;
    Prefix.clear();
    Suffix.clear();

    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        vector<vector<double>> PrefixEachPeriod;
        vector<vector<double>> SuffixEachPeriod;
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            vector<double>PrefixEachRoute;
            vector<double>SuffixEachRoute;
            
            PrefixEachPeriod.push_back(PrefixEachRoute);
            SuffixEachPeriod.push_back(SuffixEachRoute);
        }

        Prefix.push_back(PrefixEachPeriod);
        Suffix.push_back(SuffixEachPeriod);
    }

    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            double TempCumulatedDistance = 0;
            int start = 0;
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                // Calcualte cumulated_distance for concatentation
                TempCumulatedDistance = TempCumulatedDistance + IRPLR.Distance[start][IRPSolution.Route[i][j][k] + 1];
                /*cout << "Prefix:";
                for (int x = 0; x <= k; x++)
                {
                    cout << IRPSolution.Route[i][j][x] << ",";
                }
                cout << endl;
                cout << "Cumulated_distance:" << TempCumulatedDistance << endl;*/

                start = IRPSolution.Route[i][j][k] + 1;
                Prefix[i][j].push_back(TempCumulatedDistance);
            }
        }

        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {

            double TempCumulatedDistance = 0;
            int end = 0;
            for (int k = IRPSolution.Route[i][j].size() - 1; k >= 0; k--)
            {
                // Calcualte cumulated_distance for concatentation
                TempCumulatedDistance = TempCumulatedDistance + IRPLR.Distance[IRPSolution.Route[i][j][k] + 1][end];
                /*cout << "Suffix:";
                for (int x = k; x < IRPSolution.Route[i][j].size(); x++)
                {
                    cout << IRPSolution.Route[i][j][x] << ",";
                }
                cout << endl;
                cout << "Cumulated_distance:" << TempCumulatedDistance << endl;*/
                end = IRPSolution.Route[i][j][k] + 1;
                Suffix[i][j].push_back(TempCumulatedDistance);
            }
        }
    }
}

double preprocessing::PopulateSingleRoutePrefixAndSuffix(input &IRPLR, vector<int> & route)
{
    double TransportationCost=0;
    //cout << "PopulateSingleRoutePrefixAndSuffix" << endl;
    SingleRoutePrefix.clear();
    SingleRoutePrefix.clear();

    double TempCumulatedDistance = 0;
    int start = 0;
    for (int k = 0; k < route.size(); k++)
    {
        // Calcualte cumulated_distance for concatentation
        TempCumulatedDistance = TempCumulatedDistance + IRPLR.Distance[start][route[k] + 1];
        //cout << "Prefix:";
        //for (int x = 0; x <= k; x++)
        //{
         //   cout << route[x] << ",";
        //}
        //cout << endl;
        //cout << "Cumulated_distance:" << TempCumulatedDistance << endl;

        start = route[k] + 1;
        SingleRoutePrefix.push_back(TempCumulatedDistance);
        TransportationCost = TempCumulatedDistance;
    }

    TempCumulatedDistance = 0;
    int end = 0;
    for (int k = route.size() - 1; k >= 0; k--)
    {
        // Calcualte cumulated_distance for concatentation
        TempCumulatedDistance = TempCumulatedDistance + IRPLR.Distance[route[k] + 1][end];
        //cout << "Suffix:";
        //for (int x = k; x < route.size(); x++)
        //{
        //    cout << route[x] << ",";
        //}
        //cout << endl;
        //cout << "Cumulated_distance:" << TempCumulatedDistance << endl;
        end = route[k] + 1;
        SingleRouteSuffix.push_back(TempCumulatedDistance);
    }
    TransportationCost = TransportationCost + IRPLR.Distance[route[route.size()-1]+1][0];
    //cout<<"TransportationCost:"<<TransportationCost<<endl;
    /*double TransportationCostForTheDay = 0;
    if (route.size() != 0)
    {
        TransportationCostForTheDay += IRPLR.Distance[0][route[0] + 1];

        for (int k = 0; k < route.size() - 1; k++)
        {
            TransportationCostForTheDay += IRPLR.Distance[route[k] + 1][route[k + 1] + 1];
        }
        TransportationCostForTheDay += IRPLR.Distance[route[route.size() - 1] + 1][0];
    }
    cout<<"TransportationCost:"<<TransportationCost<<", "<<TransportationCostForTheDay<<endl;
    assert(abs(TransportationCost - TransportationCostForTheDay)<=0.001);*/
    return TransportationCost;
}
