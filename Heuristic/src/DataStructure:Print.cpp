#include "lib.h"
void preprocessing::PrintGlobalDataStructure()
{
    cout<<"Print Data Structure with single node"<<endl;\
    for(int i=0;i<SingleNode.size();i++)
    {
        cout<<SingleNode[i]<<",";
    }
    cout<<endl;

    cout<<"Print Data Structure with two node"<<endl;

    for(int i=0;i<TwoNodes.size();i++)
    {
        for(int j=0;j<TwoNodes[i].size();j++)
        {
            cout<<TwoNodes[i][j]<<",";
        }
        cout<<endl;
    }
}

void preprocessing::PrintPrefixAndSuffix(solution &IRPSolution)
{
}