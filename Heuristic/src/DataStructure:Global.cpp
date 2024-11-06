#include "lib.h"
void preprocessing::PopulateGlobalDataStructure(input &IRPLR)
{

    // Preprocessing subpathes with single node
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        double cumulated_distance = 0;
        SingleNode.push_back(cumulated_distance);
    }

    // Preprocessing subpathes with two node
    for (int i = 0; i < IRPLR.Retailers.size(); i++)
    {
        vector<double> tempTwoNodes;
        for (int j = 0; j < IRPLR.Retailers.size(); j++)
        {
            double cumulated_distance = SingleNode[i] + SingleNode[j] + IRPLR.Distance[i][j];
            tempTwoNodes.push_back(cumulated_distance);
        }
        TwoNodes.push_back(tempTwoNodes);
    }
}