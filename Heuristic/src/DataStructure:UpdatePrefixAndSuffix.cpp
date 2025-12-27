#include "lib.h"
void preprocessing::UpdatePrefixAndSuffix(input &IRPLR, solution &IRPSolution)
{
    for (int i = 0; i < TrackSolutionStatus.size(); i++) // day
    {
        for (int j = 0; j < TrackSolutionStatus[i].size(); j++) // vehicle
        {
            if (TrackSolutionStatus[i][j] == 1) // If route has been changed, update the prefix and suffix
            {
                Prefix[i][j].clear();
                Suffix[i][j].clear();
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

                TempCumulatedDistance = 0;
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
                TrackSolutionStatus[i][j] = 0; // Reset the status
            }
        }
    }
}
