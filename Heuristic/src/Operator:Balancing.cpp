#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR,
                                               vector<vector<vector<int>>> &TempRoute,
                                               vector<vector<int>> &TempUnallocatedCustomers,
                                               vector<vector<double>> &TempVehicleLoad,
                                               vector<vector<double>> &TempDeliveryQuantity,
                                               vector<vector<double>> &TempInventoryLevel,
                                               vector<vector<int>> &TempVehicleAllocation)
{
    cout << "Balancing quantity operator" << endl;
    double objv = 0;
    for (int i = 0; i < TempVehicleLoad.size(); i++)
    {
        for (int j = 0; j < TempVehicleLoad[i].size(); j++)
        {
            TempVehicleLoad[i][j] = 0;
        }
    }
    for (int i = 0; i < TempDeliveryQuantity.size(); i++)
    {
        for (int j = 0; j < TempDeliveryQuantity[i].size(); j++)
        {
            TempDeliveryQuantity[i][j] = 0;
        }
    }
    for (int i = 0; i < TempInventoryLevel.size(); i++)
    {
        for (int j = 0; j < TempInventoryLevel[i].size(); j++)
        {
            TempInventoryLevel[i][j] = 0;
        }
    }
    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

    return objv;
}