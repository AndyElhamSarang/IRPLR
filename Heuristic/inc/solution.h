class solution
{
private:
public:
  double TotalTransportationCost;
  double LogisticRatio;
  double TotalDelivery;
  double ViolationStockOut;
  double solution_time;

  vector<vector<vector<int>>> Route; //Main decision
  vector<vector<int>> UnallocatedCustomers;
  vector<vector<double>> VehicleLoad;
  vector<vector<double>> DeliveryQuantity; //Main decision
  vector<vector<double>> InventoryLevel;
  vector<vector<int>> VehicleAllocation;
  vector<vector<int>> VisitOrder;
  vector<double> StockOutPerCustomer;
  vector<vector<double>> TransportationCostPerRoute; // Transportation cost for each route.
  

  solution();
  ~solution();
  void Initialization(input &IRPLR);
  void OutputCVRP(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  void ReadCVRP_Solution(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  int CheckStockOut(input &IRPLR, int &RetailerIndex);

  void UpdateVehicleAllocationVisitOrder(input &IRPLR);
  void print_solution(input &IRPLR);
  void GetLogisticRatio(input &IRPLR);
  double GetTransCostSingleRoute(/*int route, int head, int tail, int insert_length, vector<int> &Input_route, data &wsrp, proprecessing &memory*/);
  void Validation(input &IRPLR);
};