#include "lib.h"
void solution::OutputCVRP(input &IRPLR, int &day, vector<vector<int>> &RouteForDay)
{
    int NumberOfCustomer = 0;
    for (int j = 0; j < RouteForDay.size(); j++)
        NumberOfCustomer += RouteForDay[j].size();

    ofstream CVRP_Instance;
    string InstanceName = "IRP";
    string FileName = MachineDirectory + "Curtin/IRPLR/HGS-CVRP-main/Instances/IRP/" + InstanceName + ".vrp";
    CVRP_Instance.open(FileName.c_str());
    CVRP_Instance << "NAME :  " << InstanceName << "\n";
    CVRP_Instance << "COMMENT : \n";
    CVRP_Instance << "TYPE :  CVRP\n";
    CVRP_Instance << "DIMENSION : \t" << NumberOfCustomer + 1 << "\n";
    CVRP_Instance << "EDGE_WEIGHT_TYPE : \tEUC_2D\n";
    CVRP_Instance << "CAPACITY : \t" << IRPLR.Vehicle.capacity << "\n";
    CVRP_Instance << "NODE_COORD_SECTION\n";
    int CustomerIndex = 1;
    CVRP_Instance << CustomerIndex << "\t" << IRPLR.Supplier.xCoord << "\t" << IRPLR.Supplier.yCoord << "\n";
    CustomerIndex++;

    for (int j = 0; j < RouteForDay.size(); j++)
    {
        for (int k = 0; k < RouteForDay[j].size(); k++)
        {
            CVRP_Instance << CustomerIndex << "\t" << IRPLR.Retailers[RouteForDay[j][k]].xCoord << "\t" << IRPLR.Retailers[RouteForDay[j][k]].yCoord << "\n";
            CustomerIndex++;
        }
    }
    CVRP_Instance << "DEMAND_SECTION\n";
    CustomerIndex = 1;
    CVRP_Instance << CustomerIndex << "\t0\n";
    CustomerIndex++;
    for (int j = 0; j < RouteForDay.size(); j++)
    {
        for (int k = 0; k < RouteForDay[j].size(); k++)
        {
            for (int z = 0; z < IRPLR.Retailers.size(); z++)
            {
                if (RouteForDay[j][k] == z)
                {

                    cout<< CustomerIndex << "\t" << DeliveryQuantity[z][day] << endl;
                    CVRP_Instance << CustomerIndex << "\t" << DeliveryQuantity[z][day] << "\n";
                    CustomerIndex++;

                    assert(DeliveryQuantity[z][day]);
                }
            }
        }
    }
    CVRP_Instance << "DEPOT_SECTION\n";
    CVRP_Instance << "\t1\n";
    CVRP_Instance << "\t-1\n";
    CVRP_Instance << "EOF\n";
    CVRP_Instance.close();
}