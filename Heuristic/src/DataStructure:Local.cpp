#include "lib.h"
void preprocessing::PopulateLocalDataStructure(input &IRPLR, solution &IRPSolution)
{

  StructureForSingleRoute.clear();//delete everything in this structure and populate new data
  
/*  for(int i=0;i<solution_in.size();i++)
    {
      
      vector< vector<info> > route;
      if(solution_in[i].size()!=0)
	{
	  for(int j=0;j<solution_in[i].size();j++)
	    {
	      
	      vector<info> head;
	      info subpath=structure1[solution_in[i][j]];
	      head.push_back(subpath);
	      if(j<solution_in[i].size()-1)
		{
		  int start=solution_in[i][j];
		  
		  for(int k=1;j+k<solution_in[i].size();k++)
		    {
		      
		      //Calcualte delta for concatenation
		      subpath.delta=subpath.D-subpath.TW+wsrp.distance[start][solution_in[i][j+k]];
		      //  cout<<"Delta:"<<subpath.delta<<endl;
	 
		      //Calculate delta_tw for concatentation
		      subpath.delta_tw=max(subpath.E+subpath.delta-structure1[solution_in[i][j+k]].L,0.0);
		      // cout<<"delta_TW:"<<subpath.delta_tw<<endl;
	 
		      //Calculate delta_wt for concatentation
		      subpath.delta_wt=max(structure1[solution_in[i][j+k]].E-subpath.delta-subpath.L,0.0);
		      // cout<<"delta_WT:"<<subpath.delta_wt<<endl;
        
		      //Calculate L for concatentation
		      subpath.L=min(structure1[solution_in[i][j+k]].L-subpath.delta,subpath.L)+subpath.delta_tw;
		      //cout<<"L:"<<subpath.L<<endl;

		      //Calculate E for concatentation
		      subpath.E=max(structure1[solution_in[i][j+k]].E-subpath.delta,subpath.E)-subpath.delta_wt;
		      //cout<<"E:"<<subpath.E<<endl;
	  
		      //Calculate D for concatentation
		      subpath.D=subpath.D+structure1[solution_in[i][j+k]].D+wsrp.distance[start][solution_in[i][j+k]]+subpath.delta_wt;
		      //cout<<"D:"<<subpath.D<<endl;
        
		      //Calculate TW for concatentation
		      subpath.TW=subpath.TW+structure1[solution_in[i][j+k]].TW+subpath.delta_tw;
		      //cout<<"TW:"<<subpath.TW<<endl;

		      //Calcualte cumulated_distance for concatentation
		      subpath.cumulated_distance=subpath.cumulated_distance+structure1[solution_in[i][j+k]].cumulated_distance+wsrp.distance[start][solution_in[i][j+k]];

		      start=solution_in[i][j+k];
		      head.push_back(subpath);
		    }	  
		}
	      route.push_back(head);
	    }
	  
	}
      structure4.push_back(route);
    }

*/

}