#include "lib.h"
void preprocessing::CustomerInfo(input &IRPLR)
{

    InitialStatusDeliveryQuantity.clear();
    InitialStatusInventoryLevel.clear();
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> TempDeliveryQuantity;
        vector<double> tempInventoryLevel;
        vector<double> TempCustomerPriority;
        double tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            TempDeliveryQuantity.push_back(0);
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand; // Delivery quantity in the initilisation phase is zero
            tempInventoryLevel.push_back(tempInventory);
            TempCustomerPriority.push_back(0);
        }
        InitialStatusDeliveryQuantity.push_back(TempDeliveryQuantity);
        InitialStatusInventoryLevel.push_back(tempInventoryLevel);
        CustomerPriority.push_back(TempCustomerPriority);
    }


     for (int i = 0; i < CustomerPriority.size(); i++)
    {
          for (int j = 0; j < CustomerPriority[i].size() ; j++)
        {
             CustomerPriority[i][j] =  IRPLR.Retailers[i].InventoryBegin/ IRPLR.Retailers[i].Demand;
        }
    }


    // cout << "Initial status:" << endl;
    // cout << "InventoryLevel:" << endl;
    // cout << "                    \tBegin\t";
    // for (int i = 0; i < IRPLR.TimeHorizon; i++)
    // {
    //     cout << "t" << i << "\t";
    // }
    // cout << endl;
    // for (int i = 0; i < InitialStatusInventoryLevel.size(); i++)
    // {
    //     cout << "Retailer " << i << "(" << IRPLR.Retailers[i].InventoryMax<<",D:"<<IRPLR.Retailers[i].Demand << ")" << ": "<<'\t' << IRPLR.Retailers[i].InventoryBegin << '\t';
    //     for (int j = 0; j < InitialStatusInventoryLevel[i].size() - 1; j++)
    //     {
    //         cout << InitialStatusInventoryLevel[i][j]<<"("<<CustomerPriority[i][j]<<")" << '\t';
    //     }
    //     cout << InitialStatusInventoryLevel[i][InitialStatusInventoryLevel[i].size() - 1]<<"("<<CustomerPriority[i][CustomerPriority[i].size() - 1]<<")" <<endl;
    // }
    // cout << endl;

}