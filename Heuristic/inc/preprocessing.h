// class for proprecessing
class preprocessing
{
private:
public:

  vector<vector<double>> InitialStatusDeliveryQuantity;
  vector<vector<double>> InitialStatusInventoryLevel;
  
  vector<vector<double>> CustomerPriority;

  vector<vector<int>> TrackSolutionStatus; // Track whether route has been changed in the solution  
  vector<vector<int>> TrackSwapTwoRoutesOnSingleDay; 
  vector<vector<int>> TrackSingleRouteOptimisation; // Track whether single route has been optimised

   vector<double> SingleNode;             // Subpathes with single node, this is a global structure, no need to update
   vector<vector<double>> TwoNodes;       ////Subpathese with two nodes, this is a global structure, no need to update
   vector<vector<vector<double>>> ThreeNodes; // Subpathes with three nodes, this is a global structure, no need to update
   vector<vector<vector<double>>> Prefix; // Subpathes begin with head, this is a local structure, keep it updated all time
   vector<vector<vector<double>>> Suffix; // Subpathes begin with tail, this is a local structure, keep it updated all time


   vector<double> SingleRoutePrefix; // For a single route, Subpathes begin with head, this is a local structure, keep it updated all time
   vector<double> SingleRouteSuffix; // For a single route, Subpathes begin with tail, this is a local structure, keep it updated all time

   vector<vector<double>> SingleRouteSubpath;

   vector<vector<vector<double>>> StructureForSingleRoute; // This is a local structure design for single route optimisation, keep it updated locally
   void PopulateGlobalDataStructure(input &IRPLR);
   void CustomerInfo(input &IRPLR);
   void PopulatePrefixAndSuffix(input &IRPLR, solution &IRPSolution);
   void UpdatePrefixAndSuffix(input &IRPLR, solution &IRPSolution);
   double PopulateSingleRoutePrefixAndSuffix(input &IRPLR, vector<int> &route);

   void PopulateSingleRouteSubpath(input &IRPLR, vector<int> &route);
   void PopulateLocalDataStructure(input &IRPLR, solution &IRPSolution);
   void PrintGlobalDataStructure();
   void PrintPrefixAndSuffix(solution &IRPSolution);
   double ConcatenateSingleRoute(int &day, int &vehicle, int &InsertTo, int &InsertFrom, int insert_length, vector<int> &Input_route, input &IRPLR, preprocessing &memory);
   double ConcatenateInter(int route1, int head, int route2, int insert_begin, int insert_length, int remove, solution &IRPSolution, input &IRPLR, preprocessing &memory );
   double ConcatenateSwapTwoRoutesSingleDay(int &day, int &route1, int &InsertTo, int &RemoveLength, int &route2, int &InsertFrom, int &InsertLength,  solution &IRPSolution, input &IRPLR, preprocessing &memory);
   double ConcatenateSwapInsertRemove(int &day, int &route, int &InsertTo, int &RemoveLength, int &InsertFrom, int &InsertLength, solution &IRPSolution, input &IRPLR, preprocessing &memory  );

   double ConcatenateChepestInsertion(int &day1, int &route1, int &InsertTo, int &RemoveLength, int &day2, int &route2, int &InsertFrom, int &InsertLength, solution &IRPSolution, input &IRPLR, preprocessing &memory);
  };
