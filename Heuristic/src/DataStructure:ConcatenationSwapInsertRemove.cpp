#include "lib.h"
double preprocessing::ConcatenateSwapInsertRemove(int &day, int &route, int &InsertTo, int &RemoveLength, int &InsertFrom, int &InsertLength, solution &IRPSolution, input &IRPLR, preprocessing &memory)
{
    /////////////////////////////////////////////////////////////////////////////////////
    //                                                                                 //
    //       This function calculate the cost of concatenation for a move invovles:    //
    //            -  Swap visiting customers with an unvisting customer                //
    //            -  insert an unvisiting customer                                     //
    //            -  remove a visiting customer                                        //
    //                                                                                 //
    //       This concatenation function invovles Operations                           //
    //              one prefix + TwoNode + one suffix.                                 //
    //                                                                                 //
    /////////////////////////////////////////////////////////////////////////////////////

    // cout << "Concatenate Swap Insert Remove called" << endl;

    /////////////////////////////////////////////////////////////
    //                                                         //
    //       Step 1: Add cost upto the inserted customer       //
    //                                                         //
    /////////////////////////////////////////////////////////////

    double TransportationCost = 0;

    if (InsertTo == 0) // if InsertTo=0 it is either the route is empty or insert at the beginning of the route
    {
        TransportationCost = 0;

        if (InsertLength == 1)
        {
            TransportationCost += IRPLR.Distance[0][IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1];
        }
        else if (InsertLength == 2)
        {
            TransportationCost += memory.TwoNodes[IRPSolution.UnallocatedCustomers[day][InsertFrom]][IRPSolution.UnallocatedCustomers[day][InsertFrom + InsertLength - 1]] +
                                  IRPLR.Distance[0][IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1];
        }
    }
    else
    {
        TransportationCost = memory.Prefix[day][route][InsertTo - 1];

        if (InsertLength == 1)
        {
            TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route][InsertTo - 1] + 1][IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1];
        }
        else if (InsertLength == 2)
        {
            TransportationCost += memory.TwoNodes[IRPSolution.UnallocatedCustomers[day][InsertFrom]][IRPSolution.UnallocatedCustomers[day][InsertFrom + InsertLength - 1]] + IRPLR.Distance[IRPSolution.Route[day][route][InsertTo - 1] + 1][IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1];
        }
    }

    /////////////////////////////////////////////////////////////
    //                                                         //
    //            Step 2: Add cost for remaining               //
    //                                                         //
    /////////////////////////////////////////////////////////////

    if (InsertLength == 1)
    {

        if (InsertTo + RemoveLength < IRPSolution.Route[day][route].size()) // test whether the job is the last job or not, if it is the last job then it will concatenate to the depot
        {
            TransportationCost += IRPLR.Distance[IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1][IRPSolution.Route[day][route][InsertTo + RemoveLength] + 1];
            TransportationCost += memory.Suffix[day][route][memory.Suffix[day][route].size() - 1 - InsertTo - RemoveLength];
        }
        else
        {
            TransportationCost += IRPLR.Distance[IRPSolution.UnallocatedCustomers[day][InsertFrom] + 1][0];
        }
    }
    else if (InsertLength == 2)
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route].size()) // test whether the job is the last job or not, it is the last job then it will concatenate to the depot
        {
            TransportationCost += IRPLR.Distance[IRPSolution.UnallocatedCustomers[day][InsertFrom + InsertLength - 1] + 1][IRPSolution.Route[day][route][InsertTo + RemoveLength] + 1];
            TransportationCost += memory.Suffix[day][route][memory.Suffix[day][route].size() - 1 - InsertTo - RemoveLength];
        }
        else
        {

            TransportationCost += IRPLR.Distance[IRPSolution.UnallocatedCustomers[day][InsertFrom + InsertLength - 1] + 1][0];
        }
    }
    else
    {
        if (InsertTo + RemoveLength < IRPSolution.Route[day][route].size()) // test whether the job is the last job or not, it is the last job then it will concatenate to the depot
        {

            if (InsertTo == 0)
            {
                TransportationCost += IRPLR.Distance[0][IRPSolution.Route[day][route][InsertTo + RemoveLength] + 1];
                TransportationCost += memory.Suffix[day][route][memory.Suffix[day][route].size() - 1 - InsertTo - RemoveLength];
            }
            else
            {
                TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route][InsertTo - 1] + 1][IRPSolution.Route[day][route][InsertTo + RemoveLength] + 1];
                TransportationCost += memory.Suffix[day][route][memory.Suffix[day][route].size() - 1 - InsertTo - RemoveLength];
            }
        }
        else
        {
            if (IRPSolution.Route[day][route].size() - RemoveLength > 0)
            {
                TransportationCost += IRPLR.Distance[IRPSolution.Route[day][route][InsertTo - 1] + 1][0];
            }
            else
            {
                TransportationCost += IRPLR.Distance[0][0];
            }
        }
    }

    return TransportationCost;
}
