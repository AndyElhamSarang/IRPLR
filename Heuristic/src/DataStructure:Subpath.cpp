#include "lib.h"
void preprocessing::PopulateSingleRouteSubpath(input &IRPLR, vector<int> &route)
{
    SingleRouteSubpath.clear(); // delete everything in this structure and populate new data
    if (route.size() != 0)
    {
        for (int j = 0; j < route.size(); j++)
        {
            vector<double> TempSingleRouteSubpath;

            int start = route[j] + 1;
            double CumulatedDistance = 0;
            TempSingleRouteSubpath.push_back(CumulatedDistance);
            //cout << "Subpath:" << route[j] << endl;
            //cout << "Cumulated_distance:" << CumulatedDistance << endl;
            if (j < route.size() - 1)
            {
                for (int k = 1; j + k < route.size(); k++)
                {
                    // Calcualte cumulated_distance for concatentation
                    CumulatedDistance = CumulatedDistance + IRPLR.Distance[start][route[j + k] + 1];
                    /*cout << "Subpath:";
                    for (int x = j; x <= j + k; x++)
                    {
                        cout << route[x] << ",";
                    }
                    cout << endl;
                    cout << "Cumulated_distance:" << CumulatedDistance << endl;*/
                    start = route[j + k] + 1;
                    TempSingleRouteSubpath.push_back(CumulatedDistance);
                }
            }
            SingleRouteSubpath.push_back(TempSingleRouteSubpath);
        }
    }
}