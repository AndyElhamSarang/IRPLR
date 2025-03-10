#include "lib.h"
void solution::Initialization(input &IRPLR)
{
    Route.clear();
    UnallocatedCustomers.clear();
    VehicleLoad.clear();
    DeliveryQuantity.clear();
    InventoryLevel.clear();
    VehicleAllocation.clear();
    VisitOrder.clear();

    int DummyVehicleAllocation = IRPLR.NumberOfVehicles + 1;

    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> TempDeliveryQuantity;
        vector<double> tempInventoryLevel;
        vector<int> tempVisitOrder;
        double tempInventory = IRPLR.Retailers[i].InventoryBegin;
        vector<int> TempVehicleAllocation;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {            
            TempDeliveryQuantity.push_back(0);
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand; //Delivery quantity in the initilisation phase is zero
            tempInventoryLevel.push_back(tempInventory);
            TempVehicleAllocation.push_back(DummyVehicleAllocation);
            tempVisitOrder.push_back(IRPLR.Retailers.size()+1);
        }
        DeliveryQuantity.push_back(TempDeliveryQuantity);
        InventoryLevel.push_back(tempInventoryLevel);
        VehicleAllocation.push_back(TempVehicleAllocation);
        VisitOrder.push_back(tempVisitOrder);
    }
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<vector<int>> temp_Route;
        vector<double> temp_VehicleLoad;
        vector<int> TempUnallocatedCustomers;
        for (int j = 0; j < IRPLR.NumberOfVehicles; j++)
        {
            temp_VehicleLoad.push_back(0); //Vehicle load in the initilisation phase is zero
            vector<int> temp_temp_route; //Route is empty
            temp_Route.push_back(temp_temp_route);
        }
        VehicleLoad.push_back(temp_VehicleLoad);
        Route.push_back(temp_Route);
        UnallocatedCustomers.push_back(TempUnallocatedCustomers);
    }
}