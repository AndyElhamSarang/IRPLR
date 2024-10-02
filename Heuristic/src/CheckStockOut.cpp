#include "lib.h"
int solution::CheckStockOut(input &IRPLR, int &RetailerIndex)
{
    int NextStockOutPeriod = IRPLR.TimeHorizon + 1;

    for (int j = 0; j < InventoryLevel[RetailerIndex].size(); j++)
    {
        if (InventoryLevel[RetailerIndex][j] < 0)
        {
            NextStockOutPeriod = j;
            break;
        }
    }

    return NextStockOutPeriod;
}