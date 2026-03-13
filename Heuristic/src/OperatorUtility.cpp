#include "lib.h"

double solution_improvement::DeliveryMax(double &VehicleCapacity, double &VehicleLoad, double &InventoryMax, double &CustomerDemand, double &PreviousInventoryLevel)
{
    // assert(VehicleCapacity - VehicleLoad > -0.00001);
    // assert(InventoryMax - PreviousInventoryLevel > -0.00001);
    // cout<<"VehicleCapacity: "<<VehicleCapacity<<", VehicleLoad: "<<VehicleLoad<<", InventoryMax: "<<InventoryMax<<", CustomerDemand: "<<CustomerDemand<<", PreviousInventoryLevel: "<<PreviousInventoryLevel<<endl;
    return max(0.0, min<double>(VehicleCapacity - VehicleLoad, InventoryMax - PreviousInventoryLevel));
}
double solution_improvement::Calculate_la_relax_objv(double &logistic_ratio, double &stockout_penalty, double &stockout)
{
    double la_relax_objv = 0.0;
    // cout<<"logistic_ratio: "<<logistic_ratio<<", stockout_penalty: "<<stockout_penalty<<", stockout: "<<stockout<<endl;
    la_relax_objv = logistic_ratio + stockout_penalty * stockout;
    return la_relax_objv;
}

void solution_improvement::AdjustQuantityAndInventoryLevel(
    double &previous_inventory_level,
    int &day,
    int &vehicle,
    vector<double> &DeliveryQuantity,
    vector<double> &InventoryLevel,
    vector<vector<double>> &VehicleLoad,
    vector<vector<int>> &VehicleAllocation,
    double &ChangeInTotalQuantity,
    double &NewStockOut,
    int &customer_index,
    input &IRPLR)
{
    assert(DeliveryQuantity.size() == IRPLR.TimeHorizon);
    assert(InventoryLevel.size() == IRPLR.TimeHorizon);
    assert(VehicleLoad.size() == IRPLR.TimeHorizon);
    assert(customer_index >= 0 && customer_index < (int)VehicleAllocation.size());
    assert(VehicleAllocation[customer_index].size() == IRPLR.TimeHorizon);

    double CurrentInventoryLevel = previous_inventory_level - IRPLR.Retailers[customer_index].Demand + DeliveryQuantity[day];
    // cout << previous_inventory_level << "," << DeliveryQuantity[day] << "," << IRPLR.Retailers[customer_index].InventoryMax << endl;
    if (previous_inventory_level + DeliveryQuantity[day] > IRPLR.Retailers[customer_index].InventoryMax)
    {
        // cout<<"& "<<previous_inventory_level<<","<<DeliveryQuantity[day]<<","<<IRPLR.Retailers[customer_index].InventoryMax<<endl;
        DeliveryQuantity[day] = DeliveryQuantity[day] - (previous_inventory_level + DeliveryQuantity[day] - IRPLR.Retailers[customer_index].InventoryMax);
        ChangeInTotalQuantity = ChangeInTotalQuantity - (previous_inventory_level + DeliveryQuantity[day] - IRPLR.Retailers[customer_index].InventoryMax);
        VehicleLoad[day][vehicle] = VehicleLoad[day][vehicle] - (previous_inventory_level + DeliveryQuantity[day] - IRPLR.Retailers[customer_index].InventoryMax);
        CurrentInventoryLevel = CurrentInventoryLevel - (previous_inventory_level + DeliveryQuantity[day] - IRPLR.Retailers[customer_index].InventoryMax);
    }

    if (fabs(InventoryLevel[day] - CurrentInventoryLevel) > 0.00001) // If Inventory level is different to the new inventory level, update the stock out accordingly.
    {
        if (InventoryLevel[day] >= 0.0 && CurrentInventoryLevel < -0.00001)
        {
            NewStockOut += -CurrentInventoryLevel;
        }
        else if (InventoryLevel[day] < -0.00001 && CurrentInventoryLevel >= 0.0)
        {
            NewStockOut -= -InventoryLevel[day];
        }
        else if (InventoryLevel[day] < -0.00001 && CurrentInventoryLevel < -0.00001)
        {
            if (InventoryLevel[day] < CurrentInventoryLevel)
            {
                NewStockOut -= fabs(InventoryLevel[day] - CurrentInventoryLevel);
            }
            else if (InventoryLevel[day] > CurrentInventoryLevel)
            {
                NewStockOut += fabs(InventoryLevel[day] - CurrentInventoryLevel);
            }
        }
    }
    // cout<< "CurrentInventoryLevel: " << CurrentInventoryLevel << endl;
    InventoryLevel[day] = CurrentInventoryLevel;
    // for (int i = 0; i < DeliveryQuantity.size(); i++)
    // {
    //     int allocVeh = VehicleAllocation[customer_index][i];
    //     if (allocVeh < 0 || allocVeh >= (int)VehicleLoad[i].size()) {
    //         cout << "NA,";
    //     } else {
    //         cout << VehicleLoad[i][allocVeh] << ",";
    //     }
    // }
    // cout << endl;
    for (int y = day + 1; y < DeliveryQuantity.size(); y++)
    {
        // cout<<"y:"<<y<<","<<DeliveryQuantity.size()<<endl;
        if (DeliveryQuantity[y] != 0) // If true means I am visiting. The algorithm assume if it is visited the delilivery quantity is positive
        {
            double DeltaQ = DeliveryMax(IRPLR.Vehicle.capacity, VehicleLoad[y][VehicleAllocation[customer_index][y]], IRPLR.Retailers[customer_index].InventoryMax, IRPLR.Retailers[customer_index].Demand, CurrentInventoryLevel);
            if (DeltaQ > 0)
            {
                int allocVeh = VehicleAllocation[customer_index][y];
                assert(allocVeh >= 0 && allocVeh < (int)VehicleLoad[y].size());
                DeliveryQuantity[y] += DeltaQ;
                ChangeInTotalQuantity += DeltaQ;
                VehicleLoad[y][allocVeh] += DeltaQ;
            }
        }
        double PreviousInventoryLevel = CurrentInventoryLevel;
        CurrentInventoryLevel = PreviousInventoryLevel - IRPLR.Retailers[customer_index].Demand + DeliveryQuantity[y];

        // cout<<"PreviousInventoryLevel: "<<PreviousInventoryLevel<<", CurrentInventoryLevel: "<<CurrentInventoryLevel<<", DeliveryQuantity[y]: "<<DeliveryQuantity[y]<<", InventoryMax: "<<IRPLR.Retailers[customer_index].InventoryMax<<endl;
        if (PreviousInventoryLevel + DeliveryQuantity[y] > IRPLR.Retailers[customer_index].InventoryMax)
        {
            // cout<<"& "<<PreviousInventoryLevel<<","<<DeliveryQuantity[y]<<","<<IRPLR.Retailers[customer_index].InventoryMax<<endl;
            double delta = (PreviousInventoryLevel + DeliveryQuantity[y] - IRPLR.Retailers[customer_index].InventoryMax);
            DeliveryQuantity[y] -= delta;
            ChangeInTotalQuantity -= delta;
            int allocVeh = VehicleAllocation[customer_index][y];
            assert(allocVeh >= 0 && allocVeh < (int)VehicleLoad[y].size());
            VehicleLoad[y][allocVeh] -= delta;
            CurrentInventoryLevel -= delta;
        }

        if (fabs(InventoryLevel[y] - CurrentInventoryLevel) > 0.00001) // If Inventory level is different to the new inventory level, update the stock out accordingly.
        {
            if (InventoryLevel[y] >= 0.0 && CurrentInventoryLevel < -0.00001)
            {
                NewStockOut += -CurrentInventoryLevel;
            }
            else if (InventoryLevel[y] < -0.00001 && CurrentInventoryLevel >= 0.0)
            {
                NewStockOut -= -InventoryLevel[y];
            }
            else if (InventoryLevel[y] < -0.00001 && CurrentInventoryLevel < -0.00001)
            {
                if (InventoryLevel[y] < CurrentInventoryLevel)
                {
                    NewStockOut -= fabs(InventoryLevel[y] - CurrentInventoryLevel);
                }
                else if (InventoryLevel[y] > CurrentInventoryLevel)
                {
                    NewStockOut += fabs(InventoryLevel[y] - CurrentInventoryLevel);
                }
            }
        }
        InventoryLevel[y] = CurrentInventoryLevel;
    }

    ////////////////////////////////////////////////////
    //                                                //
    //             Function Validation                //
    //                                                //
    ////////////////////////////////////////////////////

    // cout << "--------------------------------------------------" << endl;
    // const int id_width2 = 1;
    // const int id_width = 3;

    // cout << "          \tBegin\t";
    // for (int i = 0; i < IRPLR.TimeHorizon; i++)
    // {
    //     cout << "t" << i << '\t';
    // }
    // cout << endl;
    // cout << "Retailer " << customer_index << "(" << IRPLR.Retailers[customer_index].InventoryMax << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[customer_index].InventoryBegin;
    // for (int j = 0; j < InventoryLevel.size() - 1; j++)
    // {
    //     cout << "\t" << setw(id_width2) << "\t" << InventoryLevel[j];
    //     if (DeliveryQuantity[j] == 0)
    //     {
    //         // cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
    //         cout << "(";
    //         if (VehicleAllocation[customer_index][j] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << DeliveryQuantity[j];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")";
    //     }
    //     else
    //     {
    //         cout << "(";
    //         if (VehicleAllocation[customer_index][j] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << DeliveryQuantity[j];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")";
    //     }
    // }
    // cout << "\t" << setw(id_width2) << "\t" << InventoryLevel[InventoryLevel.size() - 1];
    // if (DeliveryQuantity[InventoryLevel.size() - 1] == 0)
    // {
    //     // cout << "(" << DeliveryQuantity[InventoryLevel.size() - 1] << ",-,-)" << endl;

    //     cout << "(";
    //     if (VehicleAllocation[customer_index][InventoryLevel.size() - 1] < IRPLR.NumberOfVehicles)
    //     {
    //         cout << DeliveryQuantity[InventoryLevel.size() - 1];
    //     }
    //     else
    //     {
    //         cout << "-";
    //     }
    //     cout << ")" << endl;
    // }
    // else
    // {
    //     cout << "(";
    //     if (VehicleAllocation[customer_index][InventoryLevel.size() - 1] < IRPLR.NumberOfVehicles)
    //     {
    //         cout << DeliveryQuantity[InventoryLevel.size() - 1];
    //     }
    //     else
    //     {

    //         cout << "-";
    //     }
    //     cout << ")" << endl;
    // }
    // cout << endl;

    // vector<double> OriginalInventoryLevel;

    // int tempInventory = IRPLR.Retailers[customer_index].InventoryBegin;
    // for (int j = 0; j < IRPLR.TimeHorizon; j++)
    // {
    //     tempInventory = tempInventory - IRPLR.Retailers[customer_index].Demand+ DeliveryQuantity[j];
    //     OriginalInventoryLevel.push_back(tempInventory);
    // }

    // cout << "Retailer " << customer_index << "(" << IRPLR.Retailers[customer_index].InventoryMax << ")" << ":\t" << setw(id_width2) << "\t" << IRPLR.Retailers[customer_index].InventoryBegin;
    // for (int j = 0; j < OriginalInventoryLevel.size() - 1; j++)
    // {
    //     cout << "\t" << setw(id_width2) << "\t" << OriginalInventoryLevel[j];
    //     if (DeliveryQuantity[j] == 0)
    //     {
    //         // cout << "(" << DeliveryQuantity[i][j] << ",-,-)\t";
    //         cout << "(";
    //         if (VehicleAllocation[customer_index][j] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << DeliveryQuantity[j];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")";
    //     }
    //     else
    //     {
    //         cout << "(";
    //         if (VehicleAllocation[customer_index][j] < IRPLR.NumberOfVehicles)
    //         {
    //             cout << DeliveryQuantity[j];
    //         }
    //         else
    //         {
    //             cout << "-";
    //         }
    //         cout << ")";
    //     }
    // }
    // cout << "\t" << setw(id_width2) << "\t" << OriginalInventoryLevel[OriginalInventoryLevel.size() - 1];
    // if (DeliveryQuantity[OriginalInventoryLevel.size() - 1] == 0)
    // {
    //     // cout << "(" << DeliveryQuantity[OriginalInventoryLevel.size() - 1] << ",-,-)" << endl;

    //     cout << "(";
    //     if (VehicleAllocation[customer_index][OriginalInventoryLevel.size() - 1] < IRPLR.NumberOfVehicles)
    //     {
    //         cout << DeliveryQuantity[OriginalInventoryLevel.size() - 1];
    //     }
    //     else
    //     {
    //         cout << "-";
    //     }
    //     cout << ")" << endl;
    // }
    // else
    // {
    //     cout << "(";
    //     if (VehicleAllocation[customer_index][OriginalInventoryLevel.size() - 1] < IRPLR.NumberOfVehicles)
    //     {
    //         cout << DeliveryQuantity[OriginalInventoryLevel.size() - 1];
    //     }
    //     else
    //     {

    //         cout << "-";
    //     }
    //     cout << ")" << endl;
    // }
    // cout << endl;

    // for(int i=0;i<InventoryLevel.size(); i++)
    // {
    //     assert(fabs(InventoryLevel[i] - OriginalInventoryLevel[i]) < 0.00001);
    // }
    // cout << "--------------------------------------------------" << endl;
}