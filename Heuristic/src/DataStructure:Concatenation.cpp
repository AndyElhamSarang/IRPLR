#include "lib.h"
double preprocessing::ConcatenateSingleRoute(int &InsertTo, int &InsertFrom, int insert_length, vector<int> &Input_route, input &IRPLR, preprocessing &memory)
{
    double TransportationCost = 0;
    //cout<<"InsertTo:"<<InsertTo<<", InsertFrom:"<< InsertFrom<<endl;

    if (InsertTo < InsertFrom)
    {
        if (InsertTo == 0)
        {
            TransportationCost = 0;

            if (insert_length == 1)
            {
                TransportationCost += IRPLR.Distance[0][Input_route[InsertFrom] + 1];
            }
            else if (insert_length == 2)
            {
                // TransportationCost += memory.structure2[Input_route[InsertFrom]][Input_route[InsertFrom + insert_length - 1]].cumulated_TransportationCost + wsrp.TransportationCost[0][Input_route[InsertFrom]];
            }
        }
        else
        {
            TransportationCost = memory.SingleRoutePrefix[InsertTo - 1];

            if (insert_length == 1)
            {
                TransportationCost += IRPLR.Distance[Input_route[InsertTo - 1] + 1][Input_route[InsertFrom] + 1];
            }
            else if (insert_length == 2)
            {
                // TransportationCost += memory.structure2[Input_route[InsertFrom]][Input_route[InsertFrom + insert_length - 1]].cumulated_TransportationCost + wsrp.TransportationCost[Input_route[InsertTo - 1]][Input_route[InsertFrom]];
            }
        }

        TransportationCost += memory.SingleRouteSubpath[InsertTo][InsertFrom - InsertTo - 1] + IRPLR.Distance[Input_route[InsertFrom + insert_length - 1] + 1][Input_route[InsertTo] + 1];

        if (InsertFrom + insert_length == Input_route.size()) // if InsertFrom is point the end
        {
            TransportationCost += IRPLR.Distance[Input_route[InsertFrom - 1] + 1][0];
        }
        else
        {
            TransportationCost += memory.SingleRouteSuffix[memory.SingleRouteSuffix.size() - 1 - InsertFrom - insert_length] +
                                  IRPLR.Distance[Input_route[InsertFrom - 1] + 1][Input_route[InsertFrom + insert_length] + 1];
        }
        // ref data from structure 1(opt1) or structure2(opt2), data for the selected subpath

        // ref data from structure 4

        // if InsertTo is point the end, ref depot data else ref suffix data
    }
    else
    {
        if (InsertFrom == 0)
        {
            TransportationCost = 0;

            TransportationCost += memory.SingleRouteSubpath[InsertFrom + insert_length][InsertTo - 1 - (InsertFrom + insert_length)] + IRPLR.Distance[0][Input_route[InsertFrom + insert_length] + 1];
        }
        else
        {
            //cout<<"!"<<endl;
            TransportationCost = memory.SingleRoutePrefix[InsertFrom - 1];

            TransportationCost += memory.SingleRouteSubpath[InsertFrom + insert_length][InsertTo - 1 - (InsertFrom + insert_length)] + IRPLR.Distance[Input_route[InsertFrom - 1] + 1][Input_route[InsertFrom + insert_length] + 1];
        }

        if (insert_length == 1)
        {
            //cout<<"!!"<<endl;
            TransportationCost += IRPLR.Distance[Input_route[InsertTo - 1] + 1][Input_route[InsertFrom] + 1];
        }
        else if (insert_length == 2)
        {
            // TransportationCost += memory.structure2[Input_route[InsertFrom]][Input_route[InsertFrom + insert_length - 1]].cumulated_TransportationCost + wsrp.TransportationCost[Input_route[InsertTo - 1]][Input_route[InsertFrom]];
        }

        if (InsertTo == Input_route.size()) // if InsertTo is point the end
        {

            //cout<<"!!!"<<endl;
            TransportationCost += IRPLR.Distance[Input_route[InsertFrom + insert_length - 1] + 1][0];
        }
        else
        {
            TransportationCost += memory.SingleRouteSuffix[memory.SingleRouteSuffix.size() - 1 - InsertTo] +
                                  IRPLR.Distance[Input_route[InsertFrom + insert_length - 1] + 1][Input_route[InsertTo] + 1];
        }

        // ref data from structure 4

        // ref data from structure 1(opt1) or structure2(opt2), data for the selected subpath

        // if InsertFrom is point the end ref depot data else ref suffix data
    }

    /*cout<< "checking"<<endl;
    vector<int> CopyOfRoute(Input_route);

    CopyOfRoute.insert(CopyOfRoute.begin() + InsertTo,
                       CopyOfRoute.begin() + InsertFrom,
                       CopyOfRoute.begin() + InsertFrom + insert_length);
    if (InsertTo < InsertFrom)
    {
        CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom + insert_length,
                          CopyOfRoute.begin() + InsertFrom + insert_length + insert_length);
    }
    else
    {
        CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom,
                          CopyOfRoute.begin() + InsertFrom + insert_length);
    }
    cout << "After move:";
    for (int i = 0; i < CopyOfRoute.size(); i++)
    {
        cout << CopyOfRoute[i] << ",";
    }
    cout << endl;

    double total_TransportationCost = 0;
    total_TransportationCost += IRPLR.Distance[0][CopyOfRoute[0] + 1];

    for (int k = 0; k < CopyOfRoute.size() - 1; k++)
    {
        total_TransportationCost += IRPLR.Distance[CopyOfRoute[k] + 1][CopyOfRoute[k + 1] + 1];
    }
    total_TransportationCost += IRPLR.Distance[CopyOfRoute[CopyOfRoute.size() - 1] + 1][0];

    cout << "Comparing TransportationCost:" << TransportationCost << "," << total_TransportationCost << endl;
    assert(abs(TransportationCost - total_TransportationCost) <= 0.001);*/

    return TransportationCost;
}