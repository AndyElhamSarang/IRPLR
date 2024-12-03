#include "lib.h"
void solution::print_solution(input &IRPLR)
{
    cout << "InventoryLevel(Delivery Quantity, Vehicle, Order):" << endl;
    cout << "                    \tBegin\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << "          \t";
    }
    cout << endl;
    for (int i = 0; i < InventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax <<","<<StockOutPerCustomer[i]<< ")" << ":    \t" << IRPLR.Retailers[i].InventoryBegin << '\t';
        for (int j = 0; j < InventoryLevel[i].size() - 1; j++)
        {
            cout << InventoryLevel[i][j];
            if (DeliveryQuantity[i][j] == 0)
            {
                //cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
                cout << "(" << DeliveryQuantity[i][j] << "," << VehicleAllocation[i][j] <<  ")    "<<'\t';
            }
            else
            {
                cout << "(" << DeliveryQuantity[i][j] << "," << VehicleAllocation[i][j] << ")    "<<'\t';

                assert(VehicleAllocation[i][j] < IRPLR.NumberOfVehicles + 1);
            }
        }
        cout << InventoryLevel[i][InventoryLevel[i].size() - 1];
        if (DeliveryQuantity[i][InventoryLevel[i].size() - 1] == 0)
        {
            //cout << "(" << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << ",-,-)" << endl;
            cout << "(" << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << "," << VehicleAllocation[i][InventoryLevel[i].size() - 1] << ")" << endl;
        }
        else
        {
            cout << "(" << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << "," << VehicleAllocation[i][InventoryLevel[i].size() - 1] << ")" << endl;
            assert(VehicleAllocation[i][InventoryLevel[i].size() - 1] < IRPLR.NumberOfVehicles + 1);
        }
    }
    cout << endl;

    cout << "Route:" << endl;
    for (int i = 0; i < Route.size(); i++)
    {
        cout << "Time period " << i << ":" << endl;
        for (int j = 0; j < Route[i].size(); j++)
        {
            cout << "Vehicle " << j << endl;
            if (Route[i][j].size() != 0)
            {
                for (int k = 0; k < Route[i][j].size() - 1; k++)
                {
                    cout << Route[i][j][k] << "(" << DeliveryQuantity[Route[i][j][k]][i] << "),";
                }
                cout << Route[i][j][Route[i][j].size() - 1] << "(" << DeliveryQuantity[Route[i][j][Route[i][j].size() - 1]][i] << ")";
            }
            cout << endl;
            cout << "Load VS Capacity: " << VehicleLoad[i][j] << "," << IRPLR.Vehicle.capacity << endl;
        }
        assert(Route[i].size() == IRPLR.NumberOfVehicles);
        cout << "Unallocated customers:";
        for (int j = 0; j < UnallocatedCustomers[i].size(); j++)
        {
            cout << UnallocatedCustomers[i][j] << ",";
        }
        cout << endl;
    }
}