#include "lib.h"
int solution_improvement::OperatorBalancing(input &IRPLR, solution &IRPSolution, vector<vector<int>> &slack, double &PenaltyForStockOut, preprocessing &memory)
{
    cout<<"Balancing quantity operator"<<endl;
    int test=0;

    for(int i=0;i<slack.size();i++)
    {
        cout<<"Slack exists in time period "<<slack[i][0]<<" at vehicle "<<slack[i][1]<<endl;
    }
    return test;
}