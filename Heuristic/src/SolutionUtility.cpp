#include "lib.h"
#include <iomanip>
#include <sstream>
void InventoryLevel_is_non_negative_and_upto_date(input &IRPLR, solution &IRPSolution)
{
   cout<<"Checking inventory level is non-negative and up-to-date."<<endl;
    // cout << "----------------------------------------------------------------------------------------------------" << endl;
    // const int id_width2 = 1;
    // const int id_width = 3;

    // cout << "          \tBegin\t";
    // for (int i = 0; i < IRPLR.TimeHorizon; i++)
    // {
    //     cout << "t" << i << '\t';
    // }
    // cout << endl;
    // for (int i = 0; i < IRPLR.Retailers.size(); i++)
    // {
    //     cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[i].InventoryBegin;
    //     for (int j = 0; j < IRPLR.TimeHorizon - 1; j++)
    //     {
    //         cout << "\t" << setw(id_width2) << "\t" << IRPSolution.InventoryLevel[i][j];
    //         if (IRPSolution.DeliveryQuantity[i][j] == 0)
    //         {
    //             // cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
    //             cout << "(";
    //             if (IRPSolution.VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
    //             {
    //                 cout << IRPSolution.DeliveryQuantity[i][j];
    //             }
    //             else
    //             {
    //                 cout << "-";
    //             }
    //             cout << ")";
    //         }
    //         else
    //         {
    //             cout << "(";
    //             if (IRPSolution.VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
    //             {
    //                 cout << IRPSolution.DeliveryQuantity[i][j];
    //             }
    //             else
    //             {
    //                 cout << "-";
    //             }
    //             cout << ")";
    //         }
    //     }

    //     cout << "\t" << setw(id_width2) << "\t" << IRPSolution.InventoryLevel[i][IRPLR.TimeHorizon - 1];
    //     if (IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1] == 0)
    //     {
    //         // cout << "(" << DeliveryQuantity[InventoryLevel.size() - 1] << ",-,-)" << endl;

    //         cout << "(";
    //         if (IRPSolution.VehicleAllocation[i][IRPLR.TimeHorizon - 1] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")" << endl;
    //     }
    //     else
    //     {
    //         cout << "(";
    //         if (IRPSolution.VehicleAllocation[i][IRPLR.TimeHorizon - 1] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1];
    //         }
    //         else
    //         {

    //             cout << "-";
    //         }
    //         cout << ")" << endl;
    //     }
    //     cout << endl;
    // }

    vector<vector<double>> OriginalInventoryLevel;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> TempOriginalInventoryLevel;
        int tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand + IRPSolution.DeliveryQuantity[i][j];
            TempOriginalInventoryLevel.push_back(tempInventory);
        }
        OriginalInventoryLevel.push_back(TempOriginalInventoryLevel);
    }

    // cout << "          \tBegin\t";
    // for (int i = 0; i < IRPLR.TimeHorizon; i++)
    // {
    //     cout << "t" << i << '\t';
    // }
    // cout << endl;
    // for (int i = 0; i < IRPLR.Retailers.size(); i++)
    // {
    //     cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[i].InventoryBegin;
    //     for (int j = 0; j < IRPLR.TimeHorizon - 1; j++)
    //     {
    //         cout << "\t" << setw(id_width2) << "\t" << OriginalInventoryLevel[i][j];
    //         if (IRPSolution.DeliveryQuantity[i][j] == 0)
    //         {
    //             // cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
    //             cout << "(";
    //             if (IRPSolution.VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
    //             {
    //                 cout << IRPSolution.DeliveryQuantity[i][j];
    //             }
    //             else
    //             {
    //                 cout << "-";
    //             }
    //             cout << ")";
    //         }
    //         else
    //         {
    //             cout << "(";
    //             if (IRPSolution.VehicleAllocation[i][j] < IRPLR.NumberOfVehicles)
    //             {
    //                 cout << IRPSolution.DeliveryQuantity[i][j];
    //             }
    //             else
    //             {
    //                 cout << "-";
    //             }
    //             cout << ")";
    //         }
    //     }

    //     cout << "\t" << setw(id_width2) << "\t" << OriginalInventoryLevel[i][IRPLR.TimeHorizon - 1];
    //     if (IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1] == 0)
    //     {
    //         // cout << "(" << DeliveryQuantity[InventoryLevel.size() - 1] << ",-,-)" << endl;

    //         cout << "(";
    //         if (IRPSolution.VehicleAllocation[i][IRPLR.TimeHorizon - 1] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")" << endl;
    //     }
    //     else
    //     {
    //         cout << "(";
    //         if (IRPSolution.VehicleAllocation[i][IRPLR.TimeHorizon - 1] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << IRPSolution.DeliveryQuantity[i][IRPLR.TimeHorizon - 1];
    //         }
    //         else
    //         {

    //             cout << "-";
    //         }
    //         cout << ")" << endl;
    //     }
    //     cout << endl;
    // }

    for (int i = 0; i < IRPSolution.InventoryLevel.size(); i++)
    {
        for (int j = 0; j < IRPSolution.InventoryLevel[i].size(); j++)
        {
            assert(fabs(IRPSolution.InventoryLevel[i][j] - OriginalInventoryLevel[i][j]) < 0.00001);
            assert(IRPSolution.InventoryLevel[i][j] >= -0.00001);
        }
    }
    // cout << "--------------------------------------------------" << endl;
    cout<<"Check passed!"<<endl;
}

void Delivery_Quantity_Cannot_Exceed_InventoryMax(input &IRPLR, solution &IRPSolution)
{
    cout<<"Checking delivery quantity cannot exceed inventory max."<<endl;
    for (int i = 0; i < IRPSolution.InventoryLevel.size(); i++)
    {
        assert(IRPLR.Retailers[i].InventoryBegin + IRPSolution.DeliveryQuantity[i][0] <= IRPLR.Retailers[i].InventoryMax + 0.00001);
        for (int j = 0; j < IRPSolution.InventoryLevel[i].size()-1; j++)
        {
            assert(IRPSolution.InventoryLevel[i][j] + IRPSolution.DeliveryQuantity[i][j+1] <= IRPLR.Retailers[i].InventoryMax + 0.00001);
        }
    }
    cout<<"Check passed!"<<endl;
}
void NumberOfVisitsPerCustomerCannotLessThanMin(input &IRPLR, solution &IRPSolution)
{
    cout<<"Check Number of Visits Per Customer Cannot Less Than Minimum Visit Demand"<<endl;
    vector<int> ListNumberOfVisits;
    for (int z = 0; z < IRPLR.Retailers.size(); z++)
    {
        int NumberOfVisits = 0;
        for (int i = 0; i < IRPSolution.Route.size(); i++)
        {
            for (int j = 0; j < IRPSolution.Route[i].size(); j++)
            {
                for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
                {
                    if (z == IRPSolution.Route[i][j][k])
                    {
                        NumberOfVisits++;
                    }
                }
            }
        }
        ListNumberOfVisits.push_back(NumberOfVisits);
    }
    for(int i = 0; i < ListNumberOfVisits.size(); i++)
    {
        // cout<<"Number of visits for customer "<<i<<": "<<ListNumberOfVisits[i]<<" versus "<<IRPLR.MinimumVisitDemand[i]<<endl;
        assert(ListNumberOfVisits[i] >= IRPLR.MinimumVisitDemand[i]);
    }
    cout<<"Check passed!"<<endl;
}
void Total_Delivery_Quantity_Per_Route_Cannot_Exceed_Capacity(input &IRPLR, solution &IRPSolution)
{
    // IRPSolution.print_solution(IRPLR);
    cout<<"Check Total Delivery Quantity Per Route Cannot Exceed Vehicle Capacity"<<endl;
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            if (IRPSolution.Route[i][j].size() != 0)
            {
                double TotalDeliveryQuantityRoute = 0;
                for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
                {
                    assert(IRPSolution.DeliveryQuantity[IRPSolution.Route[i][j][k]][i] >= -0.00001);
                    TotalDeliveryQuantityRoute += IRPSolution.DeliveryQuantity[IRPSolution.Route[i][j][k]][i];
                }
                // cout<<"TotalDeliveryQuantityRoute for day "<<i<<", vehicle "<<j<<": "<<TotalDeliveryQuantityRoute<<" versus "<<IRPLR.Vehicle.capacity<<", "<<IRPSolution.VehicleLoad[i][j]<<endl;
                assert(TotalDeliveryQuantityRoute <= IRPLR.Vehicle.capacity + 0.00001);
            }
        }
    }
    cout<<"Check passed!"<<endl;
}
void InventoryLevel_Cannot_Exceed_Maximum(input &IRPLR, solution &IRPSolution)
{
    cout<<"Check Inventory Level Cannot Exceed Maximum"<<endl;
    for (int i = 0; i < IRPSolution.InventoryLevel.size(); i++) // For each retailer
    {
        for (int j = 0; j < IRPSolution.InventoryLevel[i].size(); j++)// For each time period
        {
            if(j == 0)
            {
                // cout<<"InventoryLevel for Retailer "<<i<<", period "<<j<<": "<<IRPSolution.DeliveryQuantity[i][j]<<" versus "<<IRPLR.Retailers[i].InventoryBegin<<endl;
                assert(IRPLR.Retailers[i].InventoryBegin + IRPSolution.DeliveryQuantity[i][j] <= IRPLR.Retailers[i].InventoryMax + 0.00001);
            }
            else
            {
                // cout<<"InventoryLevel for Retailer "<<i<<", period "<<j<<": "<<IRPSolution.DeliveryQuantity[i][j]<<" versus "<<IRPLR.Retailers[i].InventoryBegin<<endl;
                assert(IRPSolution.InventoryLevel[i][j-1] + IRPSolution.DeliveryQuantity[i][j] <= IRPLR.Retailers[i].InventoryMax + 0.00001);
            }
        }
    }
    cout<<"Check passed!"<<endl;
}
void solution::Validation(input &IRPLR)
{   
    
    Total_Delivery_Quantity_Per_Route_Cannot_Exceed_Capacity(IRPLR, *this); 
    Delivery_Quantity_Cannot_Exceed_InventoryMax(IRPLR, *this);
    InventoryLevel_is_non_negative_and_upto_date(IRPLR, *this);
    NumberOfVisitsPerCustomerCannotLessThanMin(IRPLR, *this);   
    InventoryLevel_Cannot_Exceed_Maximum(IRPLR, *this);
    //Not use more than vehicle available
    //Each customer is visited once per period.
}

void solution::UpdateVehicleAllocationVisitOrder(input &IRPLR)
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
}

void solution::print_solution(input &IRPLR)
{
    

    const int id_width2 = 1;
    const int id_width = 3;
    vector<int> ListNumberOfVisits;
    for (int z = 0; z < IRPLR.Retailers.size(); z++)
    {
        int NumberOfVisits = 0;
        for (int i = 0; i < Route.size(); i++)
        {
            for (int j = 0; j < Route[i].size(); j++)
            {
                for (int k = 0; k < Route[i][j].size(); k++)
                {
                    if (z == Route[i][j][k])
                    {
                        NumberOfVisits++;
                    }
                }
            }
        }
        ListNumberOfVisits.push_back(NumberOfVisits);
    }
    cout << "InventoryLevel(Delivery Quantity, Vehicle, Order):" << endl;
    cout << "ID (Max Inv, Stockout, #Visits, Min_visits):\t" << setw(id_width) << "\tBegin\t" << setw(id_width) << "\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << "\t" << setw(id_width) << "\t";
    }
    cout << endl;
    for (int i = 0; i < InventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax << "," << StockOutPerCustomer[i] << "," << ListNumberOfVisits[i] << "," << IRPLR.MinimumVisitDemand[i] << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[i].InventoryBegin;
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