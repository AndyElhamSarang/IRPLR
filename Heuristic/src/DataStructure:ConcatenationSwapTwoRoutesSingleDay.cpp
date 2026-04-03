#include "lib.h"
double preprocessing::ConcatenateSwapTwoRoutesSingleDay(int &day, int &route1, int &InsertTo, int &RemoveLength, int &route2, int &InsertFrom, int &InsertLength, solution &IRPSolution, input &IRPLR, preprocessing &memory)
{

    double TransportationCost = 0;

    if (InsertTo == 0) // if InsertTo=0 it is either the route is empty or insert in the begining of the route
    {
        TransportationCost = 0;

        if (InsertLength != 0) // if a subpath insert into current route
        {
            if (InsertLength == 1)
            {
                TransportationCost += IRPLR.Distance[0][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }
            else if (InsertLength == 2)
            {
                TransportationCost += memory.TwoNodes[IRPSolution.Route[day][route2][InsertFrom]][IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1]] + IRPLR.Distance[0][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }
            else if (InsertLength == 3)
            {
                TransportationCost += memory.ThreeNodes[IRPSolution.Route[day][route2][InsertFrom]][IRPSolution.Route[day][route2][InsertFrom + 1]][IRPSolution.Route[day][route2][InsertFrom + 2]] + IRPLR.Distance[0][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }

        }
    }
    else
    {
        TransportationCost = memory.Prefix[day][route1][InsertTo - 1];

        if (InsertLength != 0)
        {
            if (InsertLength == 1)
            {
                TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route1][InsertTo - 1] + 1][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }
            else if (InsertLength == 2)
            {
                TransportationCost += memory.TwoNodes[IRPSolution.Route[day][route2][InsertFrom]][IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1]] + IRPLR.Distance[IRPSolution.Route[day][route1][InsertTo - 1] + 1][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }
            else if (InsertLength == 3)
            {
                TransportationCost += memory.ThreeNodes[IRPSolution.Route[day][route2][InsertFrom]][IRPSolution.Route[day][route2][InsertFrom + 1]][IRPSolution.Route[day][route2][InsertFrom + 2]] + IRPLR.Distance[IRPSolution.Route[day][route1][InsertTo - 1] + 1][IRPSolution.Route[day][route2][InsertFrom] + 1];
            }
        }
    }

    if (InsertLength == 1)
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route1].size())
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom] + 1][IRPSolution.Route[day][route1][InsertTo + RemoveLength] + 1];
            TransportationCost += memory.Suffix[day][route1][memory.Suffix[day][route1].size() - 1 - InsertTo - RemoveLength];
        }
        else
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom] + 1][0];
        }
    }
    else if (InsertLength == 2)
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route1].size()) // test whether the job is the last job or not, it is the last job then it will concatenate to the depot
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1] + 1][IRPSolution.Route[day][route1][InsertTo + RemoveLength] + 1];
            TransportationCost += memory.Suffix[day][route1][memory.Suffix[day][route1].size() - 1 - InsertTo - RemoveLength];
        }
        else
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1] + 1][0];
        }
    }
    else if (InsertLength == 3)
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route1].size()) // test whether the job is the last job or not, it is the last job then it will concatenate to the depot
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1] + 1][IRPSolution.Route[day][route1][InsertTo + RemoveLength] + 1];
            TransportationCost += memory.Suffix[day][route1][memory.Suffix[day][route1].size() - 1 - InsertTo - RemoveLength];
        }
        else
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route2][InsertFrom + InsertLength - 1] + 1][0];
        }
    }
    else
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route1].size()) // test whether the job is the last job or not, it is the last job then it will concatenate to the depot
        {
            if (InsertTo == 0)
            {
                TransportationCost += IRPLR.Distance[0][IRPSolution.Route[day][route1][InsertTo + RemoveLength] + 1];
                TransportationCost += memory.Suffix[day][route1][memory.Suffix[day][route1].size() - 1 - InsertTo - RemoveLength];
            }
            else
            {
                TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route1][InsertTo - 1] + 1][IRPSolution.Route[day][route1][InsertTo + RemoveLength] + 1];
                TransportationCost += memory.Suffix[day][route1][memory.Suffix[day][route1].size() - 1 - InsertTo - RemoveLength];
            }
        }
        else
        {
            if (IRPSolution.Route[day][route1].size() - RemoveLength > 0)
            {
                TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route1][InsertTo - 1] + 1][0];
            }
            else
            {
                TransportationCost += IRPLR.Distance[0][0];
            }
        }
    }

    return TransportationCost;
}



