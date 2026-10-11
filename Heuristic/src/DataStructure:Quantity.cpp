#include "lib.h"
void preprocessing::PopulateQuantityStructure(input &IRPLR, solution &IRPSolution)
{
    // cout << "================================" << endl;
    // cout << "PopulateQuantityStructure" << endl;
    QuantityStructure.clear();
    TotalSlackPerDay.clear();
    TotalSlackPerVehicle.clear();
    for (int customer = 0; customer < IRPLR.Retailers.size(); customer++)
    {
        vector<double> QuantityStructureForCustomer;
        for (int time = 0; time < IRPLR.TimeHorizon; time++)
        {

            double InventoryBranchout = 0;
            if (time == 0)
            {
                InventoryBranchout = IRPLR.Retailers[customer].InventoryBegin;
            }
            else
            {
                InventoryBranchout = IRPSolution.InventoryLevel[customer][time - 1];
            }
            double possible_quantity_removal = min(IRPSolution.InventoryLevel[customer][time], IRPSolution.DeliveryQuantity[customer][time]);
            double LargestStockout = 0;
            InventoryBranchout = InventoryBranchout - IRPLR.Retailers[customer].Demand + IRPSolution.DeliveryQuantity[customer][time] - possible_quantity_removal;
            // cout << InventoryBranchout << ",";
            if (LargestStockout < -InventoryBranchout)
            {
                LargestStockout = -InventoryBranchout;
            }
            for (int Subsequent_Time = time + 1; Subsequent_Time < IRPSolution.DeliveryQuantity[customer].size(); Subsequent_Time++)
            {
                InventoryBranchout = InventoryBranchout - IRPLR.Retailers[customer].Demand + IRPSolution.DeliveryQuantity[customer][Subsequent_Time];
                // cout << InventoryBranchout << ",";
                if (LargestStockout < -InventoryBranchout)
                {
                    LargestStockout = -InventoryBranchout;
                }
            }
            // cout << endl;
            possible_quantity_removal = max(0.0, possible_quantity_removal - LargestStockout);

            double quantity_removal = possible_quantity_removal;
            // cout << "Customer " << customer << " at time " << time << " on vehicle " << IRPSolution.VehicleAllocation[customer][time] << ": possible quantity removal: " << possible_quantity_removal << "; quantity to remove:" << quantity_removal << endl;
            QuantityStructureForCustomer.push_back(quantity_removal);
        }
        QuantityStructure.push_back(QuantityStructureForCustomer);
    }

    for (int time = 0; time < IRPLR.TimeHorizon; time++)
    {
        vector<double> TempTotalSlackPerVehicle;
        for (int vehicle = 0; vehicle < IRPLR.NumberOfVehicles; vehicle++)
        {
            TempTotalSlackPerVehicle.push_back(0);
        }
        TotalSlackPerVehicle.push_back(TempTotalSlackPerVehicle);
    }

    for (int time = 0; time < IRPLR.TimeHorizon; time++)
    {
        for (int customer = 0; customer < IRPLR.Retailers.size(); customer++)
        {
            cout << "Vehicle " << IRPSolution.VehicleAllocation[customer][time] << " for customer " << customer << " at time " << time << ": QuantityStructure: " << QuantityStructure[customer][time] << endl;
        }
    }

    for (int time = 0; time < IRPLR.TimeHorizon; time++)
    {
        double TempTotalSlackPerDay = 0;
        vector<double> TempTotalSlackPerVehicle;
        for (int customer = 0; customer < IRPLR.Retailers.size(); customer++)
        {
            if (IRPSolution.VehicleAllocation[customer][time] < IRPLR.NumberOfVehicles)
            {
                TotalSlackPerVehicle[time][IRPSolution.VehicleAllocation[customer][time]] += QuantityStructure[customer][time];
                TempTotalSlackPerDay += QuantityStructure[customer][time];
            }
        }
        TotalSlackPerDay.push_back(TempTotalSlackPerDay);
    }
    for (int i = 0; i < TotalSlackPerVehicle.size(); i++)
    {
        for (int j = 0; j < TotalSlackPerVehicle[i].size(); j++)
        {
            cout << "Total slack for vehicle " << j << " on day " << i << ": " << TotalSlackPerVehicle[i][j] << endl;
        }
    }

    cout << "Status of QuantityStructure(Slack):" << endl;
    for (int i = 0; i < QuantityStructure.size(); i++)
    {
        for (int j = 0; j < QuantityStructure[i].size(); j++)
        {
            cout << QuantityStructure[i][j] << ",";
        }
        cout << endl;
    }
    for (int i = 0; i < TotalSlackPerDay.size(); i++)
    {
        cout << "Total slack for day " << i << ": " << TotalSlackPerDay[i] << endl;
    }
}

void preprocessing::UpdateQuantityStructure(input &IRPLR, solution &IRPSolution, int &customer, int &time, int &vehicle)
{
    // cout << "================================" << endl;
    // cout << "UpdateQuantityStructure" << endl;

    double quantity_removal = 0;

    double InventoryBranchout = 0;
    if (time == 0)
    {
        InventoryBranchout = IRPLR.Retailers[customer].InventoryBegin;
    }
    else
    {
        InventoryBranchout = IRPSolution.InventoryLevel[customer][time - 1];
    }
    double possible_quantity_removal = min(IRPSolution.InventoryLevel[customer][time], IRPSolution.DeliveryQuantity[customer][time]);
    double LargestStockout = 0;
    InventoryBranchout = InventoryBranchout - IRPLR.Retailers[customer].Demand + IRPSolution.DeliveryQuantity[customer][time] - possible_quantity_removal;
    // cout << InventoryBranchout << ",";
    if (LargestStockout < -InventoryBranchout)
    {
        LargestStockout = -InventoryBranchout;
    }
    for (int Subsequent_Time = time + 1; Subsequent_Time < IRPSolution.DeliveryQuantity[customer].size(); Subsequent_Time++)
    {
        InventoryBranchout = InventoryBranchout - IRPLR.Retailers[customer].Demand + IRPSolution.DeliveryQuantity[customer][Subsequent_Time];
        // cout << InventoryBranchout << ",";
        if (LargestStockout < -InventoryBranchout)
        {
            LargestStockout = -InventoryBranchout;
        }
    }
    // cout << endl;
    possible_quantity_removal = max(0.0, possible_quantity_removal - LargestStockout);
    quantity_removal = possible_quantity_removal;

    // cout << "Customer " << customer << " at time " << time << ": possible quantity removal: " << possible_quantity_removal << "; quantity to remove:" << quantity_removal << endl;

    TotalSlackPerDay[time] -= QuantityStructure[customer][time];                                                    // Update the total slack for the day
    TotalSlackPerVehicle[time][vehicle] -= QuantityStructure[customer][time]; // Update the total slack for the vehicle
    QuantityStructure[customer][time] = quantity_removal;
    TotalSlackPerDay[time] += QuantityStructure[customer][time];                                                    // Update the total slack for the day
    TotalSlackPerVehicle[time][vehicle] += QuantityStructure[customer][time]; // Update the total slack for the vehicle

    //    for(int i=0;i<TotalSlackPerVehicle.size();i++)
    //     {
    //         for(int j=0;j<TotalSlackPerVehicle[i].size();j++)
    //         {
    //             cout<<"Total slack for vehicle "<<j<<" on day "<<i<<": "<<TotalSlackPerVehicle[i][j]<<endl;
    //         }
    //     }

    //     cout << "Status of QuantityStructure(Slack):" << endl;
    //     for (int i = 0; i < QuantityStructure.size(); i++)
    //     {
    //         for (int j = 0; j < QuantityStructure[i].size(); j++)
    //         {
    //             cout << QuantityStructure[i][j] << ",";
    //         }
    //         cout << endl;
    //     }
    //     for (int i = 0; i < TotalSlackPerDay.size(); i++)
    //     {
    //         cout << "Total slack for day " << i << ": " << TotalSlackPerDay[i] << endl;
    //     }
}