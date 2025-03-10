#include "lib.h"
void solution_construction::INITIAL_ZONE(input &IRPLR, solution &IRPSolution, HGS &Routing)
{
    if (printout_initial == 1)
    {
        cout << "Construct initial solution using INITIAL ZONE" << endl;
    }
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
        cout << "Output CVRP instance" << endl;
        cout << "Original solution" << endl;
        IRPSolution.print_solution(IRPLR);
    }

    ///////////////////////////////////////////////
    //                                           //
    //                Routing                    //
    //                                           //
    ///////////////////////////////////////////////

    // for (int i = 0; i < IRPSolution.Route.size(); i++)
    // {
    //     int NumberOfCustomerOfDay = 0;
    //     for (int j = 0; j < IRPSolution.Route[i].size(); j++)
    //     {
    //         NumberOfCustomerOfDay += IRPSolution.Route[i][j].size();
    //     }
    //     if (NumberOfCustomerOfDay > 1)
    //     {
    //         IRPSolution.OutputCVRP(IRPLR, i, IRPSolution.Route[i]);
    //         Routing.CallHGS(IRPLR);
    //         IRPSolution.ReadCVRP_Solution(IRPLR, i, IRPSolution.Route[i]);
    //     }
    // }
    // IRPSolution.GetLogisticRatio(IRPLR);
    // if (printout_initial == 1)
    // {
    //     cout << "Solution after Optimizing the routes" << endl;
    //     IRPSolution.print_solution(IRPLR);
    //     cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
    // }
    // if (OutputResults == 1)
    // {
    //     Table << IRPSolution.TotalTransportationCost << "," << IRPSolution.TotalDelivery << "," << IRPSolution.LogisticRatio << ",";
    // }
    // cout<<"IRPLR.NumberOfRetailers:"<<IRPLR.NumberOfRetailers<<", "<<"IRPLR.Retailers.size():"<<IRPLR.Retailers.size()<<endl;
    // for(int i=0;i<IRPSolution.VisitOrder.size();i++)
    // {
    //     for(int j=0;j<IRPSolution.VisitOrder[i].size();j++)
    //     {
    //         IRPSolution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles+1;
    //         IRPSolution.VisitOrder[i][j] = IRPLR.Retailers.size() +1;
    //     }
    // }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

}