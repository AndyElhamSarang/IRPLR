#include "lib.h"
void solution_improvement::PrintTempSolution(input &IRPLR,
                                             vector<vector<vector<int>>> &Route,
                                             vector<vector<int>> &UnallocatedCustomers,
                                             vector<vector<double>> &VehicleLoad,
                                             vector<vector<double>> &DeliveryQuantity,
                                             vector<vector<double>> &InventoryLevel,
                                             vector<vector<int>> &VehicleAllocation,
                                             vector<vector<int>> &VisitOrder)
{
    cout << "InventoryLevel(Delivery Quantity, Vehicle, Order):" << endl;
    cout << "                    \tBegin\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << "              \t";
    }
    cout << endl;
    for (int i = 0; i < InventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax << ")" << ":    \t" << IRPLR.Retailers[i].InventoryBegin << '\t';
        for (int j = 0; j < InventoryLevel[i].size() - 1; j++)
        {
            cout << InventoryLevel[i][j];
            if (DeliveryQuantity[i][j] == 0)
            {
                cout << "(";
                if (VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
                {

                    assert(VisitOrder[i][j] < IRPLR.Retailers.size());
                    cout << DeliveryQuantity[i][j] << "," << VehicleAllocation[i][j] << "," << VisitOrder[i][j];
                }
                else
                {
                    assert(VisitOrder[i][j] == IRPLR.Retailers.size() + 1);
                    cout << "-,-,-";
                }
                cout << ")        " << '\t';
            }
            else
            {
                cout << "(";
                if (VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
                {
                    assert(VisitOrder[i][j] < IRPLR.Retailers.size());
                    cout << DeliveryQuantity[i][j] << "," << VehicleAllocation[i][j] << "," << VisitOrder[i][j];
                }
                else
                {
                    assert(VisitOrder[i][j] == IRPLR.Retailers.size() + 1);
                    cout << "-,-,-";
                }
                cout << ")        " << '\t';
                assert(VehicleAllocation[i][j] < IRPLR.NumberOfVehicles + 1);
            }
        }
        cout << InventoryLevel[i][InventoryLevel[i].size() - 1];
        if (DeliveryQuantity[i][InventoryLevel[i].size() - 1] == 0)
        {
            // cout << "(" << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << ",-,-)" << endl;
            cout << "(";
            if (VehicleAllocation[i][InventoryLevel[i].size() - 1] < IRPLR.NumberOfVehicles)
            {
                assert(VisitOrder[i][InventoryLevel[i].size() - 1] < IRPLR.Retailers.size());
                cout << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << "," << VehicleAllocation[i][InventoryLevel[i].size() - 1] << "," << VisitOrder[i][InventoryLevel[i].size() - 1];
            }
            else
            {
                assert(VisitOrder[i][InventoryLevel[i].size() - 1] == IRPLR.Retailers.size() + 1);
                cout << "-,-,-";
            }
            cout << ")        " << endl;
        }
        else
        {
            cout << "(";
            if (VehicleAllocation[i][InventoryLevel[i].size() - 1] < IRPLR.NumberOfVehicles)
            {
                assert(VisitOrder[i][InventoryLevel[i].size() - 1] < IRPLR.Retailers.size());
                cout << DeliveryQuantity[i][InventoryLevel[i].size() - 1] << "," << VehicleAllocation[i][InventoryLevel[i].size() - 1] << "," << VisitOrder[i][InventoryLevel[i].size() - 1];
            }
            else
            {
                assert(VisitOrder[i][InventoryLevel[i].size() - 1] == IRPLR.Retailers.size() + 1);
                cout << "-,-,-";
            }
            cout << ")        " << endl;
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
