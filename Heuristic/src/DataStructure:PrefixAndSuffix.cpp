#include "lib.h"
void preprocessing::PopulateLocalPrefixAndSuffix(input &IRPLR, solution &IRPSolution)
{
    cout << "PopulateLocalPrefixAndSuffix" << endl;
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
                cout << "Prefix:";
                for (int x = 0; x <= k; x++)
                {
                    cout << IRPSolution.Route[i][j][x] << ",";
                }
                cout << endl;
                cout << "Cumulated_distance:" << TempCumulatedDistance << endl;

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
                cout << "Suffix:";
                for (int x = k; x < IRPSolution.Route[i][j].size(); x++)
                {
                    cout << IRPSolution.Route[i][j][x] << ",";
                }
                cout << endl;
                cout << "Cumulated_distance:" << TempCumulatedDistance << endl;
                end = IRPSolution.Route[i][j][k] + 1;
                Suffix[i][j].push_back(TempCumulatedDistance);
            }
        }
    }
}
