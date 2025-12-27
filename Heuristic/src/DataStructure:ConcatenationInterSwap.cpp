#include "lib.h"
double preprocessing::ConcatenateInter(int route1, int head, int route2, int insert_begin, int insert_length, int remove, solution &IRPSolution, input &IRPLR, preprocessing &memory )
{
    double TransportationCost = 0;


//   //Concatenate associated with one prefix, one structure 2 or structure1 and one suffix.
//   if(printout==25)
//     {
//       cout<<"route1:"<<route1<<"; head:"<<head<<"; route2:"<<route2<<"; insert_begin:"<<insert_begin<<"; insert_length"<<insert_length<<"; remove:"<<remove<<endl;
//     }
//   double value=0;
//   double distance=0;
//   double temp_delta, temp_delta_wt, temp_delta_tw, temp_L, temp_E, temp_D, temp_TW=0;

  
//   if(head==0)//if head=0 it is either the route is empty or insert in the begining of the route
//     {
//       distance=memory.structure1[0].cumulated_distance;
      
//       temp_delta=memory.structure1[0].delta;
//       temp_delta_wt=memory.structure1[0].delta_wt;
//       temp_delta_tw=memory.structure1[0].delta_tw;
//       temp_L=memory.structure1[0].L;
//       temp_E=memory.structure1[0].E;
//       temp_D=memory.structure1[0].D;
//       temp_TW=memory.structure1[0].TW;
      
//       if(insert_length!=0)//if a subpath insert into current route
// 	{
// 	  if(insert_length==1)
// 	    {
// 	      distance += memory.structure1[ Input_solution[ route2 ][ insert_begin ] ] . cumulated_distance
// 	      	+wsrp.distance[0][Input_solution[route2][insert_begin]];

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_solution[route2][insert_begin]];
	      
// 	      temp_delta_wt=max(memory.structure1[Input_solution[route2][insert_begin]].E-temp_delta-temp_L,0.0);

// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[Input_solution[route2][insert_begin]].L,0.0);
				
// 	      temp_L=min(memory.structure1[Input_solution[route2][insert_begin]].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure1[Input_solution[route2][insert_begin]].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure1[Input_solution[route2][insert_begin]].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure1[Input_solution[route2][insert_begin]].D+temp_delta_wt
// 		+wsrp.distance[0][Input_solution[route2][insert_begin]];
	      
				      
	      
// 	    }
// 	  else if(insert_length==2)
// 	    {
// 	      distance+=memory.structure2[Input_solution[route2][insert_begin]]
// 		[Input_solution[route2][insert_begin+insert_length-1]].cumulated_distance
// 	      	+wsrp.distance[0][Input_solution[route2][insert_begin]];

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_solution[route2][insert_begin]];
	      
// 	      temp_delta_wt=max(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].E-temp_delta-temp_L,0.0);

// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].L,0.0);
				
// 	      temp_L=min(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].D+temp_delta_wt+wsrp.distance[0][Input_solution[route2][insert_begin]];
// 	    }	 
// 	}
      
//     }
//   else
//     {
//       distance=memory.structure3_prefix[route1][head-1].cumulated_distance;
//       temp_delta=memory.structure3_prefix[route1][head-1].delta;
//       temp_delta_wt=memory.structure3_prefix[route1][head-1].delta_wt;
//       temp_delta_tw=memory.structure3_prefix[route1][head-1].delta_tw;
//       temp_E=memory.structure3_prefix[route1][head-1].E;
//       temp_L=memory.structure3_prefix[route1][head-1].L;
//       temp_D=memory.structure3_prefix[route1][head-1].D;
//       temp_TW=memory.structure3_prefix[route1][head-1].TW;
      
//       if(insert_length!=0)
// 	{
// 	  if(insert_length==1)
// 	    {
// 	      distance+=memory.structure1[Input_solution[route2][insert_begin]].cumulated_distance
// 		+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];

// 	      temp_delta_wt=max(memory.structure1[Input_solution[route2][insert_begin]].E-temp_delta-temp_L,0.0);

// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[Input_solution[route2][insert_begin]].L,0.0);
	      
// 	      temp_L=min(memory.structure1[Input_solution[route2][insert_begin]].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure1[Input_solution[route2][insert_begin]].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure1[Input_solution[route2][insert_begin]].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure1[Input_solution[route2][insert_begin]].D+temp_delta_wt
// 		+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];
			
// 	    }
// 	  else if(insert_length==2)
// 	    {
// 	      distance+=memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].cumulated_distance
// 		+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];
	      

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];

// 	      temp_delta_wt=max(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].E-temp_delta-temp_L,0.0);

// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].L,0.0);

// 	      temp_L=min(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure2[Input_solution[route2][insert_begin]][Input_solution[route2][insert_begin+insert_length-1]].D+temp_delta_wt
// 		+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route2][insert_begin]];
// 	    }
	 
// 	}    
//     }

  

//   //  if(Input_solution[route1].size()!=0)
//   //   {
//   if(insert_length==1)
//     {   
//       if(head+remove<Input_solution[route1].size())//test whether the job is the last job or not, it is the last job then it will concatenate to the depot
// 	{
// 	  //assert(memory.structure3_suffix[route1].size()-1-head-remove>=0);
// 	  distance+=wsrp.distance[Input_solution[route2][insert_begin]][Input_solution[route1][head+remove]];	  
// 	  distance+=memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].cumulated_distance;

// 	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route2][insert_begin]][Input_solution[route1][head+remove]];
	  
// 	  temp_delta_wt=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta-temp_L,0.0);
	  
// 	  temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L,0.0);

// 	  temp_L=min(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L-temp_delta,temp_L)+temp_delta_tw;

// 	  temp_E=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta,temp_E)-temp_delta_wt;

// 	  temp_TW=temp_TW+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].TW+temp_delta_tw;

// 	  temp_D=temp_D+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].D+temp_delta_wt+wsrp.distance[Input_solution[route2][insert_begin]][Input_solution[route1][head+remove]];
// 	}
//       else
// 	{
// 	  distance+=wsrp.distance[Input_solution[route2][insert_begin]][0]+memory.structure1[0].cumulated_distance;

// 	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route2][insert_begin]][0];
	  
// 	  temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);
	  
// 	  temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);

// 	  temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

// 	  temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

// 	  temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

// 	  temp_D=temp_D+memory.structure1[0].D+temp_delta_wt+wsrp.distance[Input_solution[route2][insert_begin]][0];	  
// 	}  
//     }
//   else if(insert_length==2)
//     {
//       if(head+remove<Input_solution[route1].size())//test whether the job is the last job or not, it is the last job then it will concatenate to the depot
// 	{
// 	  distance+=wsrp.distance[Input_solution[route2][insert_begin+1]][Input_solution[route1][head+remove]];
// 	  distance+=memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].cumulated_distance;

// 	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route2][insert_begin+1]][Input_solution[route1][head+remove]];
	  
// 	  temp_delta_wt=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta-temp_L,0.0);
	  
// 	  temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L,0.0);

// 	  temp_L=min(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L-temp_delta,temp_L)+temp_delta_tw;

// 	  temp_E=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta,temp_E)-temp_delta_wt;

// 	  temp_TW=temp_TW+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].TW+temp_delta_tw;

// 	  temp_D=temp_D+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].D+temp_delta_wt+wsrp.distance[Input_solution[route2][insert_begin+1]][Input_solution[route1][head+remove]];
   
// 	}
//       else
// 	{

	 
// 	  distance+=wsrp.distance[Input_solution[route2][insert_begin+1]][0]+memory.structure1[0].cumulated_distance;

// 	   temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route2][insert_begin+1]][0];
	  
// 	  temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);
	  
// 	  temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);

// 	  temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

// 	  temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

// 	  temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

// 	  temp_D=temp_D+memory.structure1[0].D+temp_delta_wt+wsrp.distance[Input_solution[route2][insert_begin+1]][0];
// 	}
      
     
//     }
//   else
//     {
//       if(head+remove<Input_solution[route1].size())//test whether the job is the last job or not, it is the last job then it will concatenate to the depot
// 	{
// 	  if(head==0)
// 	    {
// 	      distance+=wsrp.distance[0][Input_solution[route1][head+remove]];
// 	      distance+=memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].cumulated_distance;

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_solution[route1][head+remove]];
	  
// 	      temp_delta_wt=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta-temp_L,0.0);
	  
// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L,0.0);

// 	      temp_L=min(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].D+temp_delta_wt+wsrp.distance[0][Input_solution[route1][head+remove]];
	      
// 	    }
// 	  else
// 	    {
// 	      //assert(head-1>=0);
// 	      distance+=wsrp.distance[Input_solution[route1][head-1]][Input_solution[route1][head+remove]];
// 	      distance+=memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].cumulated_distance;

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route1][head+remove]];
	  
// 	      temp_delta_wt=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta-temp_L,0.0);
	  
// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L,0.0);

// 	      temp_L=min(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure3_suffix[route1][memory.structure3_suffix[route1].size()-1-head-remove].D+temp_delta_wt+wsrp.distance[Input_solution[route1][head-1]][Input_solution[route1][head+remove]];
// 	    }
	
	  
// 	}
//       else
// 	{
// 	  //assert(head!=0);
// 	  if(Input_solution[route1].size()!=0)
// 	    {
// 	      distance+=wsrp.distance[Input_solution[route1][head-1]][0]+memory.structure1[0].cumulated_distance;
	    
// 	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_solution[route1][head-1]][0];
	  
// 	      temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);
	  
// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);

// 	      temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure1[0].D+temp_delta_wt+wsrp.distance[Input_solution[route1][head-1]][0];
// 	    }
// 	  else
// 	    {
	      
// 	      distance+=wsrp.distance[0][0]+memory.structure1[0].cumulated_distance;

// 	      temp_delta=temp_D-temp_TW+wsrp.distance[0][0];
	  
// 	      temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);
	  
// 	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);

// 	      temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

// 	      temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

// 	      temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

// 	      temp_D=temp_D+memory.structure1[0].D+temp_delta_wt+wsrp.distance[0][0];

		
// 	    }
	    
// 	}
	     
//     }

//   //  }

//   /*if(printout==25)
//     {    
//       for(int i=0;i<head;i++)
// 	{
// 	  cout<<wsrp.taskList[Input_solution[route1][i]].customerID<<",";
// 	}
//       for(int i=0;i<insert_length;i++)
// 	{
      
// 	  cout<<wsrp.taskList[Input_solution[route2][insert_begin+i]].customerID<<",";
// 	}
 
//       for(int i=head+remove;i<Input_solution[route1].size();i++)
// 	{
// 	  cout<<wsrp.taskList[Input_solution[route1][i]].customerID<<",";
// 	}
//       cout<<endl;
//     }
      
      
  
//    vector< vector<int> > as(Input_solution);
					  
  
//       as[route1].insert(as[route1].begin()+head,
// 			as[route2].begin()+insert_begin,
// 			as[route2].begin()+insert_begin+insert_length);
//       as[route2].erase(as[route2].begin()+insert_begin,
// 		       as[route2].begin()+insert_begin+insert_length);
						  
//       as[route2].insert(as[route2].begin()+insert_begin,
// 			as[route1].begin()+head+insert_length,
// 			as[route1].begin()+head+insert_length+remove);
//       as[route1].erase(as[route1].begin()+head+insert_length,
// 		       as[route1].begin()+head+insert_length+remove);



   
//       double total_distance=0;
   
//        for(int y=0;y<head+insert_length;y++)
// 	 {
// 	 cout<<wsrp.taskList[as[route1][y]].customerID<<",";
// 	 }
// 	 cout<<endl;
   
   
   
  
// 	 if(head+insert_length!=0)
// 	 {
// 	 if(head+insert_length>=1)
// 	 {
// 	 for(int j=1;j<head+insert_length;j++)
// 	 {
		 
// 	 total_distance+=wsrp.distance[as[route1][j]][as[route1][j-1]];
// 	 }
// 	 }

     
// 	 total_distance+=wsrp.distance[0][as[route1][0]];
 
 
    
// 	 //total_distance+=wsrp.distance[0][as[route1][as[route1].size()-1]];
// 	 }
     
//       for(int y=0;y<as[route1].size();y++)
// 	{
// 	  cout<<wsrp.taskList[as[route1][y]].customerID<<",";
// 	}
//       cout<<endl;
	 
//       if(as[route1].size()!=0)
// 	{
// 	  if(as[route1].size()>=2)
// 	    {
// 	      for(int j=1;j<as[route1].size();j++)
// 		{
		 
// 		  total_distance+=wsrp.distance[as[route1][j]][as[route1][j-1]];
// 		}
// 	    }

     
// 	  total_distance+=wsrp.distance[0][as[route1][0]];
 
 
    
// 	  total_distance+=wsrp.distance[0][as[route1][as[route1].size()-1]];
// 	}
   

//       cout<<"Naive distance:"<<total_distance<<endl;
    

//       cout<<endl;

//       if(fabs(total_distance-distance)>=0.00001)
// 	{
// 	  cout<<"error"<<endl;
// 	  }
// 	  // }
      
//       double wd_violation=0;
//       double tw_violation=0;
//       if(as[route1].size()>0)
// 	{
// 	  //Initialize starting time
// 	  double time=0;
// 	  double time_arrive_at_first_customer=time+wsrp.distance[0][as[route1][0]];
// 	  double starting_time=0;
// 	  if(wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer>0)
// 	    {
// 	      time=wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer;
// 	      starting_time=wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer;
// 	    }
// 	  else 
// 	    {
// 	      time=0;
// 	      starting_time=0;
// 	    }
// 	  // cout<<"starting time"<<starting_time<<endl;
      
// 	  int route=0;
     
// 	  for(int i=0;i<head+insert_length;i++)
// 	    {
// 	      time=time+wsrp.distance[as[route1][i]][route];
// 	      if(time<wsrp.taskList[as[route1][i]].aTime)
// 		{
	     
// 		  time=wsrp.taskList[as[route1][i]].aTime;
	      
// 		}
// 	      if(time>wsrp.taskList[as[route1][i]].bTime)
// 		{
// 		  tw_violation+=time-wsrp.taskList[as[route1][i]].bTime;
	      
// 		  time=wsrp.taskList[as[route1][i]].bTime;
// 		}
	 
// 	      time=time+wsrp.taskList[as[route1][i]].sTime;
	 
	  
// 	      route=as[route1][i];
// 	    }
// 	  // time=time+wsrp.distance[as[route1][as[route1].size()-1]][0];
     
      
    
// 	}
//       if(as[route1].size()>0)
// 	{
// 	  //Initialize starting time
// 	  double time=0;
// 	  double time_arrive_at_first_customer=time+wsrp.distance[0][as[route1][0]];
// 	  double starting_time=0;
// 	  if(wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer>0)
// 	    {
// 	      time=wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer;
// 	      starting_time=wsrp.taskList[as[route1][0]].aTime-time_arrive_at_first_customer;
// 	    }
// 	  else 
// 	    {
// 	      time=0;
// 	      starting_time=0;
// 	    }
// 	  // cout<<"starting time"<<starting_time<<endl;
      
// 	  int route=0;
// 	  double cumulated_time=0;
// 	  for(int i=0;i<head+insert_length;i++)
// 	    {
// 	      time=time+wsrp.distance[as[route1][i]][route];
// 	      cumulated_time+=wsrp.distance[as[route1][i]][route];
// 	      if(time<wsrp.taskList[as[route1][i]].aTime)
// 		{
// 		  cumulated_time+=wsrp.taskList[as[route1][i]].aTime-time;
// 		  time=wsrp.taskList[as[route1][i]].aTime;
	      
// 		}
// 	      if(time>wsrp.taskList[as[route1][i]].bTime)
// 		{
// 		  time=wsrp.taskList[as[route1][i]].bTime;
// 		}
	 
// 	      time=time+wsrp.taskList[as[route1][i]].sTime;
// 	      cumulated_time+=wsrp.taskList[as[route1][i]].sTime;
	  
// 	      route=as[route1][i];
// 	    }
// 	  // time=time+wsrp.distance[as[route1][as[route1].size()-1]][0];
// 	  // cumulated_time+=wsrp.distance[as[route1][as[route1].size()-1]][0];
      
// 	  //cout<<wsrp.work_duration<<","<<time+tw_violation<<",";
// 	  if(cumulated_time>wsrp.work_duration)
// 	    {
// 	      wd_violation=cumulated_time-wsrp.work_duration;
// 	    }

      
// 	}
 
//       // cout<<"Exit time_duration_violation"<<endl;
//       //cout<<"tdv"<<",";
 

//       cout<<"TW:"<<temp_TW<<","<<tw_violation;
//       if(fabs(temp_TW-tw_violation)>0.00001)
// 	{
// 	  cout<<"There is difference on time window";
// 	}
//       cout<<endl;
//       cout<<"D:"<<temp_D<<","<<wd_violation;
//       if(fabs(temp_D-wd_violation)>0.00001)
// 	{
// 	  cout<<"There is difference on duration";
// 	}
//       cout<<endl;*/


      
      
      
//   if(temp_D>wsrp.work_duration)
//     {
//       temp_D=temp_D-wsrp.work_duration;
//     }
//   else
//     {
//       temp_D=0;
//     }

//   if(temp_TW<0.0001)
//     {
//       temp_TW=0;
//     }
  

  
//   value+=distance+multiplier[0]*temp_TW+multiplier[1]*temp_D;
  
    return TransportationCost;
}
