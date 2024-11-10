#include "lib.h"
void solution_improvement::LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing,preprocessing &memory)
{
    memory.PopulateLocalPrefixAndSuffix(IRPLR, IRPSolution);

    vector<vector<double>> OriginalInventoryLevel;
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> tempInventoryLevel;
        int tempInventory = IRPLR.Retailers[i].InventoryBegin;
        for (int j = 0; j < IRPLR.TimeHorizon; j++)
        {
            tempInventory = tempInventory - IRPLR.Retailers[i].Demand;
            tempInventoryLevel.push_back(tempInventory);
        }
        OriginalInventoryLevel.push_back(tempInventoryLevel);
    }
    cout << "OriginalInventoryLevel:" << endl;
    cout << "          \tBegin\t";
    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        cout << "t" << i << '\t';
    }
    cout << endl;
    for (int i = 0; i < OriginalInventoryLevel.size(); i++)
    {
        cout << "Retailer " << i << ":\t" << IRPLR.Retailers[i].InventoryBegin << '\t';
        for (int j = 0; j < OriginalInventoryLevel[i].size() - 1; j++)
        {
            cout << OriginalInventoryLevel[i][j] << '\t';
        }
        cout << OriginalInventoryLevel[i][OriginalInventoryLevel[i].size() - 1] << endl;
    }
    cout << endl;

    IRPSolution.print_solution(IRPLR);
    vector<vector<double>> PossibleDeliveryQuantityDeduction; 
    vector<int> NumberOfVisits;
    for(int i=0;i<IRPLR.Retailers.size();i++)
    {
        NumberOfVisits.push_back(0);
    }
   
    for(int i=0;i<IRPSolution.Route.size();i++)
    {
        for(int j=0;j<IRPSolution.Route[i].size();j++)
        {
            for(int k=0;k<IRPSolution.Route[i][j].size();k++)
            {
                for (int x=0;x<IRPLR.Retailers.size();x++)
                {
                    if(IRPSolution.Route[i][j][k] == x)
                    {
                        NumberOfVisits[x]++;
                    }
                }
            }
        }

    }

    cout<<"NumberOfVisits"<<endl;
    for(int i=0;i<NumberOfVisits.size();i++)
    {
        cout<<NumberOfVisits[i]<<",";
    }
    cout<<endl;
    
double violation = OperatorSwap(IRPLR, IRPSolution, Routing, PenaltyForStockOut);
       
    /*int OperatorSwapCounter = 0;
    double PenaltyForStockOut = 1;
    while (OperatorSwapCounter <= 10)
    {
        double violation = OperatorSwap(IRPLR, IRPSolution, Routing, PenaltyForStockOut);
        IRPSolution.GetLogisticRatio(IRPLR);
        cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << endl;
        if (violation != 0)
        {
            PenaltyForStockOut = PenaltyForStockOut * 2;
        }
        else
        {
            PenaltyForStockOut = PenaltyForStockOut / 2;
        }

        OperatorSwapCounter++;
    }*/
    // LNS_Destory(IRPLR, IRPSolution,Routing);
    // LNS_Repair(IRPLR, IRPSolution,Routing);
}
