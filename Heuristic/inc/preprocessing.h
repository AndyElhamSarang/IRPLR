//class for proprecessing
class preprocessing
{
 private:

 
  public:
   vector<double> SingleNode;
   vector<vector<double>> TwoNodes;

   void PopulateGlobalDataStructure(input &IRPLR);

   void PrintGlobalDataStructure();
   /*vector<info> structure1;//Subpathese with single node, this is a global structure, keep it updated all time
   vector< vector< info > > structure2;//Subpathese with 2 nodes, this is a global structure, keep it updated all time
   vector< vector<info> > structure3_prefix;//Subpathese begin with head, this is a global structure, keep it updated all time
   vector< vector<info> > structure3_suffix;//Subpathese begin with tail, this is a global structure, keep it updated all time
   vector< vector< vector<info> > > structure4;//This is a local structure design for single route optimisation, keep it updated locally
   
   vector<int> Keep_track_on_the_solution;//record modified routes and update in the stage2 proprocessing
   void initialize_tracking(int &a);
   void clear_tracking();
   void stage2();
   void pop_structure2(data &wsrp, int &firstPath, int &secPath, vector<double> &relevantData);
   void pop_structure3(data &wsrp, vector< vector<int> > &solution_in);
   void pop_structure4(data &wsrp, vector< vector<int> > &solution_in);
   //void update_structure3(data &wsrp, vector< vector<int> > &Input_solution);
   //void update_structure4(data &wsrp, vector< vector<int> > &Input_solution);
   //void check_stage1(data &wsrp);
   //void check_stage2(data &wsrp, vector< vector<int> > &solution_in);
   //void check_structure4(data &wsrp, vector< vector<int> > &solution_in); */
};
