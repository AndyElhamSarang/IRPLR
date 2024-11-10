#include "lib.h"
double solution::GetTransCostSingleRoute(/*int route, int head, int tail, int insert_length, vector<int> &Input_route, data &wsrp, proprecessing &memory*/)
{
   double value=0;
  /*double distance=0;
 
   if(head<tail)
    {
      if(head==0)
	{
	  distance=memory.structure1[0].cumulated_distance;
      
	  temp_delta=memory.structure1[0].delta;
	  temp_delta_wt=memory.structure1[0].delta_wt;
	  temp_delta_tw=memory.structure1[0].delta_tw;
	  temp_L=memory.structure1[0].L;
	  temp_E=memory.structure1[0].E;
	  temp_D=memory.structure1[0].D;
	  temp_TW=memory.structure1[0].TW;

	  
	  if(insert_length==1)
	    {
	      distance+=memory.structure1[Input_route[tail]].cumulated_distance
	      	+wsrp.distance[0][Input_route[tail]];

	      temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_route[tail]];
	      
	      temp_delta_wt=max(memory.structure1[Input_route[tail]].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[Input_route[tail]].L,0.0);
				
	      temp_L=min(memory.structure1[Input_route[tail]].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure1[Input_route[tail]].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure1[Input_route[tail]].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure1[Input_route[tail]].D+temp_delta_wt
		+wsrp.distance[0][Input_route[tail]];
	    }
	  else if(insert_length==2)
	    {
	       distance+=memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].cumulated_distance
	      	+wsrp.distance[0][Input_route[tail]];

	      temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_route[tail]];
	      
	      temp_delta_wt=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L,0.0);
				
	      temp_L=min(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].D+temp_delta_wt+wsrp.distance[0][Input_route[tail]];


	      
	    }
	  
	  

	}
      else
	{
	  distance=memory.structure3_prefix[route][head-1].cumulated_distance;

	  temp_delta=memory.structure3_prefix[route][head-1].delta;
	  temp_delta_wt=memory.structure3_prefix[route][head-1].delta_wt;
	  temp_delta_tw=memory.structure3_prefix[route][head-1].delta_tw;
	  temp_E=memory.structure3_prefix[route][head-1].E;
	  temp_L=memory.structure3_prefix[route][head-1].L;
	  temp_D=memory.structure3_prefix[route][head-1].D;
	  temp_TW=memory.structure3_prefix[route][head-1].TW;

	   if(insert_length==1)
	    {
	      distance+=memory.structure1[Input_route[tail]].cumulated_distance
	      	+wsrp.distance[Input_route[head-1]][Input_route[tail]];

	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[head-1]][Input_route[tail]];
	      
	      temp_delta_wt=max(memory.structure1[Input_route[tail]].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[Input_route[tail]].L,0.0);
				
	      temp_L=min(memory.structure1[Input_route[tail]].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure1[Input_route[tail]].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure1[Input_route[tail]].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure1[Input_route[tail]].D+temp_delta_wt
		+wsrp.distance[Input_route[head-1]][Input_route[tail]];
	    }
	  else if(insert_length==2)
	    {
	       distance+=memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].cumulated_distance
	      	+wsrp.distance[Input_route[head-1]][Input_route[tail]];

	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[head-1]][Input_route[tail]];
	      
	      temp_delta_wt=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L,0.0);
				
	      temp_L=min(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].D+temp_delta_wt+wsrp.distance[Input_route[head-1]][Input_route[tail]];


	      
	    }

	  
	}

     
      
	  distance+=memory.structure4[route][head][tail-head-1].cumulated_distance
	    +wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];

	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];
	      
	  temp_delta_wt=max(memory.structure4[route][head][tail-head-1].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure4[route][head][tail-head-1].L,0.0);
				
	  temp_L=min(memory.structure4[route][head][tail-head-1].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure4[route][head][tail-head-1].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure4[route][head][tail-head-1].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure4[route][head][tail-head-1].D+temp_delta_wt+wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];



      

	  if(tail+insert_length==Input_route.size())// if tail is point the end
	    {
	      distance+=memory.structure1[0].cumulated_distance
	      	+wsrp.distance[Input_route[tail-1]][0];

	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail-1]][0];
	      
	      temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);
				
	      temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure1[0].D+temp_delta_wt
		+wsrp.distance[Input_route[tail-1]][0];


	    }
	  else
	    {
	      distance+=memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].cumulated_distance+
		wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];

		
	      temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];
	      
	      temp_delta_wt=max(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].E-temp_delta-temp_L,0.0);

	      temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].L,0.0);
				
	      temp_L=min(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].L-temp_delta,temp_L)+temp_delta_tw;

	      temp_E=max(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].E-temp_delta,temp_E)-temp_delta_wt;

	      temp_TW=temp_TW+memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].TW+temp_delta_tw;

	      temp_D=temp_D+memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-tail-insert_length].D+temp_delta_wt
		+wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];
		

	    }
      //ref data from structure 1(opt1) or structure2(opt2), data for the selected subpath
      
      //ref data from structure 4

      //if head is point the end, ref depot data else ref suffix data
      
    }
  else
    {
      if(tail==0)
	{
	  distance=memory.structure1[0].cumulated_distance;
      
	  temp_delta=memory.structure1[0].delta;
	  temp_delta_wt=memory.structure1[0].delta_wt;
	  temp_delta_tw=memory.structure1[0].delta_tw;
	  temp_L=memory.structure1[0].L;
	  temp_E=memory.structure1[0].E;
	  temp_D=memory.structure1[0].D;
	  temp_TW=memory.structure1[0].TW;

	  distance+=memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].cumulated_distance
	    +wsrp.distance[0][Input_route[tail+insert_length]];

	  temp_delta=temp_D-temp_TW+wsrp.distance[0][Input_route[tail+insert_length]];
	      
	  temp_delta_wt=max(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].L,0.0);
				
	  temp_L=min(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].D+temp_delta_wt+wsrp.distance[0][Input_route[tail+insert_length]];


	}
      else
	{
	  distance=memory.structure3_prefix[route][tail-1].cumulated_distance;

	  temp_delta=memory.structure3_prefix[route][tail-1].delta;
	  temp_delta_wt=memory.structure3_prefix[route][tail-1].delta_wt;
	  temp_delta_tw=memory.structure3_prefix[route][tail-1].delta_tw;
	  temp_E=memory.structure3_prefix[route][tail-1].E;
	  temp_L=memory.structure3_prefix[route][tail-1].L;
	  temp_D=memory.structure3_prefix[route][tail-1].D;
	  temp_TW=memory.structure3_prefix[route][tail-1].TW;

	 distance+=memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].cumulated_distance
	    +wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];

	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];
	      
	  temp_delta_wt=max(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].L,0.0);
				
	  temp_L=min(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure4[route][tail+insert_length][head-1-(tail+insert_length)].D+temp_delta_wt+wsrp.distance[Input_route[tail-1]][Input_route[tail+insert_length]];

	  
	  
	}

      if(insert_length==1)
	{
	  distance+=memory.structure1[Input_route[tail]].cumulated_distance
	    +wsrp.distance[Input_route[head-1]][Input_route[tail]];

	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[head-1]][Input_route[tail]];
	      
	  temp_delta_wt=max(memory.structure1[Input_route[tail]].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure1[Input_route[tail]].L,0.0);
				
	  temp_L=min(memory.structure1[Input_route[tail]].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure1[Input_route[tail]].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure1[Input_route[tail]].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure1[Input_route[tail]].D+temp_delta_wt
	    +wsrp.distance[Input_route[head-1]][Input_route[tail]];
	}
      else if(insert_length==2)
	{
	  distance+=memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].cumulated_distance
	    +wsrp.distance[Input_route[head-1]][Input_route[tail]];

	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[head-1]][Input_route[tail]];
	      
	  temp_delta_wt=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L,0.0);
				
	  temp_L=min(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure2[Input_route[tail]][Input_route[tail+insert_length-1]].D+temp_delta_wt+wsrp.distance[Input_route[head-1]][Input_route[tail]];


	      
	}
      
      if(head==Input_route.size())// if head is point the end
	{
	  distance+=memory.structure1[0].cumulated_distance
	    +wsrp.distance[Input_route[tail+insert_length-1]][0];

	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail+insert_length-1]][0];
	      
	  temp_delta_wt=max(memory.structure1[0].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure1[0].L,0.0);
				
	  temp_L=min(memory.structure1[0].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure1[0].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure1[0].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure1[0].D+temp_delta_wt
	    +wsrp.distance[Input_route[tail+insert_length-1]][0];


	}
      else
	{
	  distance+=memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].cumulated_distance+
	    wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];

		
	  temp_delta=temp_D-temp_TW+wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];
	      
	  temp_delta_wt=max(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].E-temp_delta-temp_L,0.0);

	  temp_delta_tw=max(temp_E+temp_delta-memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].L,0.0);
				
	  temp_L=min(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].L-temp_delta,temp_L)+temp_delta_tw;

	  temp_E=max(memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].E-temp_delta,temp_E)-temp_delta_wt;

	  temp_TW=temp_TW+memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].TW+temp_delta_tw;

	  temp_D=temp_D+memory.structure3_suffix[route][memory.structure3_suffix[route].size()-1-head].D+temp_delta_wt
	  +wsrp.distance[Input_route[tail+insert_length-1]][Input_route[head]];
		

	}
    


      //ref data from structure 4
      
      //ref data from structure 1(opt1) or structure2(opt2), data for the selected subpath

      //if tail is point the end ref depot data else ref suffix data
    }
  
  

   if(temp_D>wsrp.work_duration)
     {
       temp_D=temp_D-wsrp.work_duration;
     }
   else
     {
       temp_D=0;
     }
  
  if(temp_TW<0.0001)
    {
      temp_TW=0;
    }
  
 
  
   if(printout==27)
     {
       //checking
       vector<int>  cs(Input_route);

       // cs.erase(cs.begin()+head,cs.end());
       //cs.insert(cs.begin()+head,
       //	    cs.begin()+tail,
       //	    cs.begin()+tail+insert_length);
       //if(tail<head)
       //{  
		
       //cs.erase(cs.begin()+head+insert_length,
       //       cs.end());
       //}
       //else
       // {
       //  cs.erase(cs.begin()+head+insert_length,cs.end());
       // cs.erase(cs.begin()+tail,
       //	       cs.begin()+tail+insert_length);     
       //}
  
       //
       cs.insert(cs.begin()+head,
		 cs.begin()+tail,
		 cs.begin()+tail+insert_length);
       if(head<tail)
	 {  
		
	   cs.erase(cs.begin()+tail+insert_length,
		    cs.begin()+tail+insert_length+insert_length);
	 }
       else
	 {	
	   cs.erase(cs.begin()+tail,
		    cs.begin()+tail+insert_length);
	 }
       //
       cout<<"After move:";
       for(int i=0;i<cs.size();i++)
	 {
	   cout<<wsrp.taskList[cs[i]].customerID<<",";
	 }
       cout<<endl;
       //
       vector<int> ds(cs);

       //concatenate once
       // if(head<tail)
       //   {
       //     ds.erase(ds.begin()+head+insert_length,ds.end());
       //   }
       // else
       //   {
       //      ds.erase(ds.begin()+head-insert_length,ds.end());
							
       //   }
   
       //concatenate twice
       // if(head<tail)
       // {
       // ds.erase(ds.begin()+tail+insert_length,ds.end());
       // }
       // else
       //{
       //   ds.erase(ds.begin()+head,ds.end());							
       //}

   
       cout<<"check:";
       for(int i=0;i<ds.size();i++)
	 {
	   cout<<wsrp.taskList[ds[i]].customerID<<",";
	 }
       cout<<endl;
       double total_distance=0;
  
       if(ds.size()!=0)
	 {
	   if(ds.size()>=2)
	     {
	       for(int j=1;j<ds.size();j++)
		 {
	     
		   total_distance+=wsrp.distance[ds[j]][ds[j-1]];
		 }
	     }
    
	   total_distance+=wsrp.distance[0][ds[0]];
 
 
     
	   total_distance+=wsrp.distance[0][ds[ds.size()-1]];
	 }
   
       cout<<"Comparing distance:"<<distance<<","<<total_distance<<",";
       if(fabs(distance-total_distance)>0.00001)
	 {
	   cout<<"There is difference";
	 }
       else
	 {
	   cout<<"it is correct";
	 }
       cout<<endl;
     }
   

  
   value+=distance+multiplier[0]*temp_TW+multiplier[1]*temp_D;*/
 
    
   return value;
		
}