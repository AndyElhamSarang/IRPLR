class solution
{
  friend class solution_construction;
private:
public:
struct assignment
  {
    double DeliveryQuantity;
    double InventoryLevel;
    int VehicleAllocation;
    int Position;
  };
  double TotalTransportationCost;
  double LogisticRatio;
  double TotalDelivery;
  double ViolationStockOut;
  
  vector<vector<vector<int>>> Route;
  vector<vector<int>> UnallocatedCustomers;
  vector<vector<double>> VehicleLoad;
  vector<vector<assignment>> Schedule;

  solution();
  ~solution();
  void Initialization(input &IRPLR);
  void OutputCVRP(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  void ReadCVRP_Solution(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  int CheckStockOut(input &IRPLR, int &RetailerIndex);
  void print_solution(input &IRPLR);
  void GetLogisticRatio(input &IRPLR);
  double GetTransCostSingleRoute(/*int route, int head, int tail, int insert_length, vector<int> &Input_route, data &wsrp, proprecessing &memory*/);

};