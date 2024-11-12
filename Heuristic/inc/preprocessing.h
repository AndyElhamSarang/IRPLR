// class for proprecessing
class preprocessing
{
private:
public:
   vector<double> SingleNode;             // Subpathes with single node, this is a global structure, no need to update
   vector<vector<double>> TwoNodes;       ////Subpathese with two nodes, this is a global structure, no need to update
   vector<vector<vector<double>>> Prefix; // Subpathes begin with head, this is a local structure, keep it updated all time
   vector<vector<vector<double>>> Suffix; // Subpathes begin with tail, this is a local structure, keep it updated all time

   vector<double> SingleRoutePrefix; // For a single route, Subpathes begin with head, this is a local structure, keep it updated all time
   vector<double> SingleRouteSuffix; // For a single route, Subpathes begin with tail, this is a local structure, keep it updated all time

   vector<vector<double>> SingleRouteSubpath;

   vector<vector<vector<double>>> StructureForSingleRoute; // This is a local structure design for single route optimisation, keep it updated locally
   void PopulateGlobalDataStructure(input &IRPLR);
   void PopulatePrefixAndSuffix(input &IRPLR, solution &IRPSolution);
   double PopulateSingleRoutePrefixAndSuffix(input &IRPLR, vector<int> &route);

   void PopulateSingleRouteSubpath(input &IRPLR, vector<int> &route);
   void PopulateLocalDataStructure(input &IRPLR, solution &IRPSolution);
   void PrintGlobalDataStructure();
   void PrintPrefixAndSuffix(solution &IRPSolution);
   double ConcatenateSingleRoute(int &InsertTo, int &InsertFrom, int insert_length, vector<int> &Input_route, input &IRPLR, preprocessing &memory);
};
