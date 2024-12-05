#include "lib.h"
void solution_improvement::LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory)
{
    // memory.PopulateLocalPrefixAndSuffix(IRPLR, IRPSolution);

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
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        NumberOfVisits.push_back(0);
    }

    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                for (int x = 0; x < IRPLR.Retailers.size(); x++)
                {
                    if (IRPSolution.Route[i][j][k] == x)
                    {
                        NumberOfVisits[x]++;
                    }
                }
            }
        }
    }

    cout << "NumberOfVisits" << endl;
    for (int i = 0; i < NumberOfVisits.size(); i++)
    {
        cout << NumberOfVisits[i] << ",";
    }
    cout << endl;

    time_t start_time;
    time_t end_time;
    time(&start_time);
    solution BestIRP_Solution(IRPSolution);
    BestIRP_Solution.GetLogisticRatio(IRPLR);
    cout << "Best logistic ratio:" << BestIRP_Solution.LogisticRatio << endl;
    int OperatorSwapCounter = 0;
    double PenaltyForStockOut = 1;
    int ToAdjustPenalty = 10;
    int ItForCurrentPenalty = 0;
    int NumberOfInfeasibleSolution = 0;
    int NumberOfFeasibleSolution = 0;
    int FeasibleSolutionCounter = 0;
    int BetterFeasibleSolutionCounter = 0;
    while (OperatorSwapCounter < 1)
    {
        int whether_improved_swap = OperatorSwap(IRPLR, IRPSolution, Routing, PenaltyForStockOut, memory);
        

        IRPSolution.GetLogisticRatio(IRPLR);
        cout << "Solution after iteration " << OperatorSwapCounter << endl;
        cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
        cout << "ViolationStockOut" << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << endl;
  IRPSolution.print_solution(IRPLR);
       
if(IRPSolution.ViolationStockOut!=0)
{
        int whether_improved_insert = OperatorInsert(IRPLR, IRPSolution,  PenaltyForStockOut, memory);
}
        // violation = OperatorRemove(IRPLR, IRPSolution,  PenaltyForStockOut, memory);
        
       ItForCurrentPenalty++;
        if (IRPSolution.ViolationStockOut - 0 > 0.001)
        {
            NumberOfInfeasibleSolution++;
            if (NumberOfInfeasibleSolution >= ToAdjustPenalty)
            {
                PenaltyForStockOut = PenaltyForStockOut * 2;
                ItForCurrentPenalty = 0;
                NumberOfInfeasibleSolution = 0;
            }
        }
        else
        {
            NumberOfFeasibleSolution++;
            FeasibleSolutionCounter++;
            cout << "Feasible solution obtained" << endl;
            if (BestIRP_Solution.LogisticRatio - IRPSolution.LogisticRatio > 0.00001)
            {
                BetterFeasibleSolutionCounter++;
                cout << "Best solution is updated" << endl;
                BestIRP_Solution = IRPSolution;
            }
            if (NumberOfFeasibleSolution >= ToAdjustPenalty)
            {
                PenaltyForStockOut = PenaltyForStockOut / 2;
                ItForCurrentPenalty = 0;
                NumberOfFeasibleSolution = 0;
            }
        }
        OperatorSwapCounter++;
    }
    time(&end_time);
    double total_time = difftime(end_time, start_time);
    double check_LogisticRatio = BestIRP_Solution.LogisticRatio;
    cout << "Best known results" << endl;
    BestIRP_Solution.print_solution(IRPLR);
    BestIRP_Solution.GetLogisticRatio(IRPLR);
    assert(fabs(check_LogisticRatio - BestIRP_Solution.LogisticRatio) < 0.001);
    cout << "!FeasibleSolutionCounter: " << FeasibleSolutionCounter << endl;
    cout << "!BetterFeasibleSolutionCounter: " << BetterFeasibleSolutionCounter << endl;
    cout << "!Best Logistic Ratio: " << BestIRP_Solution.LogisticRatio << endl;
    cout << "!Time: " << total_time << endl;
    // LNS_Destory(IRPLR, IRPSolution,Routing);
    // LNS_Repair(IRPLR, IRPSolution,Routing);
}
