#include "lib.h"
double solution_improvement::OperatorSingleRoute(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut)
{
  double violation = 0;
  return violation;
}




int solution_improvement::opt1(/* vector<int>  &as,  vector<int>  &bs, int i, data &wsrp, obj &obj_subg, proprecessing & memory*/)
{
  int whether_improve_move_been_found=0;
/*
  //cout<<"o1"<<",";
  // if(printout==10){cout<<"Enter opt1"<<endl;}

  double input_route_distance = 0;
  double opt1obj=obj_subg.single_route_lagrangian_objective_efficient_revised(wsrp, memory,as,i, input_route_distance);
  double accepting_level = numeric_limits<double>::max();
  double input_obj = opt1obj;

  //cout<<"start o:"<<input_route_distance << " l:"<<input_obj<<endl;
 
  //if(printout==10){
  //  cout<<"****Starting point******"<<endl;   
  //  for(int y=0;y<as.size();y++)
  //    {
  //	cout<<wsrp.taskList[as[y]].customerID<<",";
  //   }
  // cout<<endl;
  //}
  vector<int> move;
  for(int move_temp=0;move_temp<3;move_temp++)
    {
      move.push_back(0);
    }
  int total_moves=0;
  int moves=0;
  //if(printout==29){
  //   if(as.size()!=0)
  //    {
  //	total_moves=as.size()*(as.size()-1);
  //    }
  // }
    
  for(int j=0;j<as.size();j++)
    {
      for(int k=0;k<=as.size();k++)
	{
	  if((k!=j+1)&&(k!=j))
	    //if(solution[i][k]!=solution[i][j])
	    {
	      // if(printout==29){moves++;}

	      double route_distance=0;
	      double routeobj=obj_subg.Concatenate_single_route_revised(i,k,j,1,as,wsrp,memory, route_distance);
	      // cout<<routeobj<<endl;
	      if(input_obj - routeobj > 0.00001)
		{

		  if(accepting_level - route_distance > 0.00001)
		    {
		      whether_improve_move_been_found=1;
		     
		      opt1obj=routeobj;

		      accepting_level = route_distance;

		      //cout<<"update o:"<<accepting_level <<" l:"<<opt1obj<<endl;
			  
		      move[0]=k;
		      move[1]=j; 
		      move[2]=1;
		    }
		  else if(fabs(accepting_level - route_distance) <0.00001)
		    {
			  
		      if(fabs(routeobj-opt1obj)>0.00001)
			{
			  if(opt1obj - routeobj > 0.00001)
			    {
			      whether_improve_move_been_found=1;
				  
			      opt1obj=routeobj;

			      accepting_level = route_distance;

			      
			      //cout<<"update o:"<<accepting_level <<" l:"<<opt1obj<<endl;
				  
			      move[0]=k;
			      move[1]=j; 
			      move[2]=1;
			    }
			}
		    }
		}/*
	      /* if(printout==10)
		{
		  vector<int>  cs;
		  cs=as;
		  double cobj1=0;
		  cs.insert(cs.begin()+k,
			    cs.begin()+j,
			    cs.begin()+j+1);
 
		  if(k<j)
		    {  
		
		      cs.erase(cs.begin()+j+1,
			       cs.begin()+j+1+1);
		    }
		  else
		    {	
		      cs.erase(cs.begin()+j,
			       cs.begin()+j+1);
		    }
		  
		  double temp_obj=obj_subg.obj_LRP(cs,i,wsrp);
		 
	
		  cout<<routeobj<<","<<temp_obj<<'\t';
		  if(fabs(temp_obj-routeobj)>=0.001)
		    {
		  
		      cout<<"There is difference";
		    }
		  else
		    {
		      cout<<"It is correct";
		    }
		  cout<<endl;
		}*/
	   


	 /*   }

	}

    }
  // if(printout==29){
  //cout<<"opt1:"<<moves<<","<<total_moves<<endl;
  //  assert(moves==total_moves);
  // }
  if(whether_improve_move_been_found!=0)
    {
     
     
    
      memory.Keep_track_on_the_solution[i]=1;
     
      
      bs.insert(bs.begin()+move[0], as.begin()+move[1], as.begin()+move[1]+move[2]);//Strange 
      
      if(move[0]<move[1])
	{
	  bs.erase(bs.begin()+move[1]+move[2],bs.begin()+move[1]+move[2]+move[2]);
	}
      else
	{
	  bs.erase(bs.begin()+move[1],bs.begin()+move[1]+move[2]);
	}
    
    }*/

  /* if(printout==10){cout<<"Exit opt1"<<endl;
    for(int i=0;i<bs.size();i++)
      {
	cout<<wsrp.taskList[bs[i]].customerID<<",";
      }
    cout<<endl;
    cout<<endl;
    }*/
  return whether_improve_move_been_found;

}
  
int solution_improvement::opt2(/*vector<int>  &as,  vector<int>  &bs,  int i, data &wsrp, obj &obj_subg, proprecessing & memory*/)
{
  int whether_improve_move_been_found=0;
  /*
  //cout<<"o2"<<",";
  // cout<<"Enter opt2"<<endl;
  double opt2obj=0;
  double input_route_distance = 0;
  opt2obj=obj_subg.single_route_lagrangian_objective_efficient_revised(wsrp,memory,as,i, input_route_distance);
  double accepting_level = numeric_limits<double>::max();
  double input_obj = opt2obj;
  // if(printout==11){
  //  cout<<"****Starting point******"<<endl;  
  //  for(int y=0;y<as.size();y++)
  //   {
  //	cout<<wsrp.taskList[as[y]].customerID<<",";
  //    }
  //  cout<<endl;
  // }
  //cout<<"start o:"<<input_route_distance << " l:"<<input_obj<<endl;
 
  vector<int> move;
  for(int move_temp=0;move_temp<3;move_temp++)
    {
      move.push_back(0);
    }
  int total_moves=0;
  int moves=0;
  //if(printout==29)
  // {   
  //  if(as.size()!=0)
  //	{
  //	  total_moves=(as.size()-1)*(as.size()-2);
  //	}
  //}
  for(int j=0;j<as.size();j++)
    {
      for(int k=0;k<=as.size();k++)
	{
	  if((k!=j+2)&&(k!=j+1)&&(k!=j)&&(j+2<=as.size()))
	    //if(solution[i][k]!=solution[i][j])
	    {
	      //if(printout==29){moves++;}
	      double route_distance=0;
	      double routeobj=obj_subg.Concatenate_single_route_revised(i,k,j,2,as,wsrp,memory,route_distance);

	      if(input_obj - routeobj > 0.00001)
		{		  
		  if(accepting_level - route_distance > 0.00001)
		    {
		      whether_improve_move_been_found=1;
		      // cout<<"found improved move in opt2"<<endl;
		      opt2obj=routeobj;

		      accepting_level = route_distance;
		      
		      move[0]=k;
		      move[1]=j;
		      move[2]=2;

		      //cout<<"update o:"<<accepting_level <<" l:"<<opt2obj<<endl;
			
		    }
		  else if(fabs(accepting_level - route_distance) < 0.00001)
		    {
		      if(fabs(routeobj-opt2obj)>0.00001)
			{
			  if(opt2obj - routeobj > 0.00001)
			    {
			      
			      whether_improve_move_been_found=1;
			      // cout<<"found improved move in opt2"<<endl;
			      opt2obj=routeobj;

			      accepting_level = route_distance;
			      
			      move[0]=k;
			      move[1]=j;
			      move[2]=2;
			      // cout<<"update o:"<<accepting_level <<" l:"<<opt2obj<<endl;
				 
			    }
			  
			}
		    }
		}*/
	      /*  if(printout==11)
		{
		  vector<int>  cs(as);
	      
		  double cobj1=0;
		  cs.insert(cs.begin()+k,
			    cs.begin()+j,
			    cs.begin()+j+2);
		  if(k<j)
		    {  
		
		      cs.erase(cs.begin()+j+2,
			       cs.begin()+j+2+2);
		    }
		  else
		    {	
		      cs.erase(cs.begin()+j,
			       cs.begin()+j+2);
		    }

		  double temp_obj=obj_subg.obj_LRP(cs,i,wsrp);
		  cout<<routeobj<<","<<temp_obj<<'\t';
		  if(fabs(temp_obj-routeobj)>=0.001)
		    {
		  
		      cout<<"There is difference";
		    }
		  else
		    {
		      cout<<"It is correct";
		    }
		  cout<<endl;
		}*/
	   /* }
	} 
    }
  // if(printout==29)
  //  {
  //cout<<"opt2:"<<moves<<","<<total_moves<<endl;
  //   assert(moves==total_moves);
  // }
  if(whether_improve_move_been_found!=0)
    {
      //if(printout==11)
      //	{
      //	  for(int i=0;i<bs.size();i++)
      //	    { 
      //      cout<<wsrp.taskList[bs[i]].customerID<<",";
      //    }
      //	  cout<<endl;
      //}
      
      memory.Keep_track_on_the_solution[i]=1;
      
      //if(printout==11){cout<<"move:"<<move[0]<<","<<move[1]<<","<<move[2]<<","<<bs.size()<<endl;}
      

      bs.insert(bs.begin()+move[0], as.begin()+move[1], as.begin()+move[1]+move[2]);//Strange 
      
      if(move[0]<move[1])
	{
	  bs.erase(bs.begin()+move[1]+move[2],bs.begin()+move[1]+move[2]+move[2]);
	}
      else
	{
	  bs.erase(bs.begin()+move[1],bs.begin()+move[1]+move[2]);
	}


      
      //if(printout==11)
      //	{
      // for(int i=0;i<bs.size();i++)
      //   {
      //    cout<<wsrp.taskList[bs[i]].customerID<<",";
      //   }
      // cout<<endl;
      //}
      
    }
  // cout<<"Exit opt2"<<endl;*/
  return whether_improve_move_been_found;
}