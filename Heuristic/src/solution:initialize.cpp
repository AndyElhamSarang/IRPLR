#include "lib.h"
void solution::Initialization(input &IRPLR)
{


    Route.clear();
    UnallocatedCustomers.clear();
    VehicleLoad.clear();
    Schedule.clear();

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> TempDeliveryQuantity;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            assignment Temp;
            
            TempDeliveryQuantity.push_back(0);
        }
        IRPSolution.DeliveryQuantity.push_back(TempDeliveryQuantity);
    }

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> tempInventoryLevel;
        int tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand + IRPSolution.DeliveryQuantity[i][j];
            tempInventoryLevel.push_back(tempInventory);
        }
        IRPSolution.InventoryLevel.push_back(tempInventoryLevel);
    }


    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<double> temp_VehicleLoad;
        for (int j = 0; j < IRPLR.NumberOfVehicles; j++)
        {
            temp_VehicleLoad.push_back(0);
        }
        IRPSolution.VehicleLoad.push_back(temp_VehicleLoad);
    }

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<vector<int>> temp_Route;
        for (int j = 0; j < IRPLR.NumberOfVehicles; j++)
        {
            vector<int> temp_temp_route;
            temp_Route.push_back(temp_temp_route);
        }
        IRPSolution.Route.push_back(temp_Route);
    }
}