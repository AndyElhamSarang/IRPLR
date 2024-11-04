#include "lib.h"
void solution::GetLogisticRatio(input &IRPLR)
{
    TotalTransportationCost = 0;
    TotalDelivery = 0;
    LogisticRatio = 0;
    ViolationStockOut = 0;
    for (int i = 0; i < Route.size(); i++)
    {
        double TransportationCostForTheDay = 0;
        for (int j = 0; j < Route[i].size(); j++)
        {

            if (Route[i][j].size() != 0)
            {
                TransportationCostForTheDay += IRPLR.Distance[0][Route[i][j][0] + 1];

                for (int k = 0; k < Route[i][j].size() - 1; k++)
                {
                    TransportationCostForTheDay += IRPLR.Distance[Route[i][j][k] + 1][Route[i][j][k + 1] + 1];
                }
                TransportationCostForTheDay += IRPLR.Distance[Route[i][j][Route[i][j].size() - 1] + 1][0];
            }
        }
        // cout<<"TransportationCostForTheDay:"<<TransportationCostForTheDay<<endl;
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
        for(int j=0;j<InventoryLevel[i].size();j++)
        {
            if(InventoryLevel[i][j]<0)
            {
                ViolationStockOut-=InventoryLevel[i][j];
            }
        }
    }
}