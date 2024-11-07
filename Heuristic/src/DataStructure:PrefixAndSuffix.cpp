#include "lib.h"
void preprocessing::PopulateLocalPrefixAndSuffix(input &IRPLR, solution &IRPSolution)
{

    Prefix.clear();
    Suffix.clear();

    /*for (int i = 0; i < solution_in.size(); i++)
    {
        vector<info> list_prefix_temp;
        info prefix_temp(structure1[0]);
        int start = 0;

        for (int j = 0; j < solution_in[i].size(); j++)
        {
            // Calcualte cumulated_distance for concatentation
            prefix_temp.cumulated_distance = prefix_temp.cumulated_distance + structure1[solution_in[i][j]].cumulated_distance + wsrp.distance[start][solution_in[i][j]];
            // cout<<"Cumulated_distance:"<<prefix_temp.cumulated_distance<<endl;

            start = solution_in[i][j];
            list_prefix_temp.push_back(prefix_temp);
        }
        Prefix.push_back(list_prefix_temp);

        vector<info> list_suffix_temp;
        info suffix_temp(structure1[0]);
        int end = 0;
        for (int j = solution_in[i].size() - 1; j >= 0; j--)
        {
               // Calcualte cumulated_distance for concatentation
            suffix_temp.cumulated_distance = suffix_temp.cumulated_distance + structure1[solution_in[i][j]].cumulated_distance + wsrp.distance[solution_in[i][j]][end];

            end = solution_in[i][j];
            list_suffix_temp.push_back(suffix_temp);
        }

        Suffix.push_back(list_suffix_temp);
    }*/
}