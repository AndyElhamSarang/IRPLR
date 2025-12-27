#include "lib.h"
#include <iomanip>
#include <sstream>
void Total_Delivery_Quantity_Per_Route_Cannot_Exceed_Capacity(input &IRPLR, solution &IRPSolution)
{
    IRPSolution.print_solution(IRPLR);
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            if (IRPSolution.Route[i][j].size() != 0)
            {
                double TotalDeliveryQuantityRoute = 0;
                for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
                {
                    TotalDeliveryQuantityRoute += IRPSolution.DeliveryQuantity[IRPSolution.Route[i][j][k]][i];
                }
                // cout<<"TotalDeliveryQuantityRoute for day "<<i<<", vehicle "<<j<<": "<<TotalDeliveryQuantityRoute<<" versus "<<IRPLR.Vehicle.capacity<<", "<<IRPSolution.VehicleLoad[i][j]<<endl;
                assert(TotalDeliveryQuantityRoute <= IRPLR.Vehicle.capacity + 0.00001);
            }
        }
    }
}
void solution::Validation(input &IRPLR)
{   
    Total_Delivery_Quantity_Per_Route_Cannot_Exceed_Capacity(IRPLR, *this);    
}

void solution::print_solution(input &IRPLR)
{
    ///////////////////////////////////////////////////////////////////////
    //                                                                   //
    //     Update vehicle allocation and visit order before printing     //
    //                                                                   //
    ///////////////////////////////////////////////////////////////////////

    for (int i = 0; i < VisitOrder.size(); i++)
    {
        for (int j = 0; j < VisitOrder[i].size(); j++)
        {
            VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
        }
    }

    for (int i = 0; i < Route.size(); i++) // For this time period
    {
        for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
        {
            for (int j = 0; j < Route[i].size(); j++) // index j for vehicle
            {
                for (int k = 0; k < Route[i][j].size(); k++) // index k for position
                {

                    if (Route[i][j][k] == x)
                    {
                        VisitOrder[x][i] = k;
                    }
                }
            }
        }
    }

    const int id_width2 = 1;

    const int id_width = 3;
    cout << "InventoryLevel(Delivery Quantity, Vehicle, Order):" << endl;
    cout << "ID (Max Inv, Stockout):\t" << setw(id_width) << "\tBegin\t" << setw(id_width) << "\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << "\t" << setw(id_width) << "\t";
    }
    cout << endl;
    for (int i = 0; i < InventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax << "," << StockOutPerCustomer[i] << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < InventoryLevel[i].size() - 1; j++)
        {
            cout << "\t" << setw(id_width2) << "\t" << InventoryLevel[i][j];
            if (DeliveryQuantity[i][j] == 0)
            {
                // cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
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
                cout << ")";
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
                cout << ")";
                assert(VehicleAllocation[i][j] < IRPLR.NumberOfVehicles + 1);
            }
        }
        cout << "\t" << setw(id_width2) << "\t" << InventoryLevel[i][InventoryLevel[i].size() - 1];
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
            cout << ")" << endl;
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
            cout << ")" << endl;
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


void solution::GetLogisticRatio(input &IRPLR)
{
    StockOutPerCustomer.clear();
    TotalTransportationCost = 0;
    TotalDelivery = 0;
    LogisticRatio = 0;
    ViolationStockOut = 0;
    TransportationCostPerRoute.clear();
    //  for (int i = 0; i < VisitOrder.size(); i++)
    // {
    //     for (int j = 0; j < VisitOrder[i].size(); j++)
    //     {
    //         VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
    //         VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
    //     }
    // }

    // for (int i = 0; i < Route.size(); i++) // For this time period
    // {
    //     for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
    //     {
    //         int UnallocatedYesOrNo = 0;
    //         for (int j = 0; j < Route[i].size(); j++) // index j for vehicle
    //         {
    //             for (int k = 0; k < Route[i][j].size(); k++) // index k for position
    //             {

    //                 if (Route[i][j][k] == x)
    //                 {
    //                     UnallocatedYesOrNo = 1;
    //                     VehicleAllocation[x][i] = j;
    //                     VisitOrder[x][i] = k;
    //                 }
    //             }
    //         }
    //     }
    // }
    for (int i = 0; i < Route.size(); i++)
    {
        double TransportationCostForTheDay = 0;
        vector<double> TransportationCostForPerDay;
        for (int j = 0; j < Route[i].size(); j++)
        {
            double TransportationCostForTheDayPerRoute = 0;
            if (Route[i][j].size() != 0)
            {
                TransportationCostForTheDayPerRoute += IRPLR.Distance[0][Route[i][j][0] + 1];

                for (int k = 0; k < Route[i][j].size() - 1; k++)
                {
                    TransportationCostForTheDayPerRoute += IRPLR.Distance[Route[i][j][k] + 1][Route[i][j][k + 1] + 1];
                }
                TransportationCostForTheDayPerRoute += IRPLR.Distance[Route[i][j][Route[i][j].size() - 1] + 1][0];
            }
            TransportationCostForPerDay.push_back(TransportationCostForTheDayPerRoute);
            TransportationCostForTheDay += TransportationCostForTheDayPerRoute;
        }
        TransportationCostPerRoute.push_back(TransportationCostForPerDay);
        TotalTransportationCost += TransportationCostForTheDay;
    }
    for (int i = 0; i < DeliveryQuantity.size(); i++)
    {
        for (int j = 0; j < DeliveryQuantity[i].size(); j++)
        {
            TotalDelivery += DeliveryQuantity[i][j];
        }
    }
    LogisticRatio = TotalTransportationCost / TotalDelivery;

    for(int i=0;i<InventoryLevel.size();i++)
    {
        double StockOutOfThisCustomer=0;
        for(int j=0;j<InventoryLevel[i].size();j++)
        {
            if(InventoryLevel[i][j]<0)
            {
                ViolationStockOut-=InventoryLevel[i][j];
                StockOutOfThisCustomer-=InventoryLevel[i][j];
            }
        }
        StockOutPerCustomer.push_back(StockOutOfThisCustomer);
    }
    // Validation(IRPLR);
}