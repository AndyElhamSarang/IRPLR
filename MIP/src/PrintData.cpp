#include "lib.h"

void input::PrintData()
{
    cout << "NumberOfVehicles:" << NumberOfVehicles << endl;
    cout << "Supplier " << Supplier.ID << ":" << Supplier.xCoord << '\t' << Supplier.yCoord << '\t' << Supplier.InventoryBegin << '\t' << Supplier.QuantityProduced << '\t' << Supplier.InventoryCost << endl;

    for (int i = 0; i < Retailers.size(); i++)
    {
        cout << "Retailer " << Retailers[i].ID << ":" << Retailers[i].xCoord << '\t' << Retailers[i].yCoord
             << '\t' << Retailers[i].InventoryBegin << '\t' << Retailers[i].InventoryMax << '\t' << Retailers[i].InventoryMin << '\t' << Retailers[i].Demand << '\t' << Retailers[i].InventoryCost << endl;
    }
    cout << "Distance matrix:" << endl;
    for (int i = 0; i < Distance.size(); i++)
    {
        for (int j = 0; j < Distance[i].size() - 1; j++)
        {
            cout << Distance[i][j] << '\t';
        }
        cout << Distance[i][Distance[i].size() - 1] << endl;
    }
}