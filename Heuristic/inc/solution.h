class solution
{
private:
public:
  double TotalTransportationCost;
  double LogisticRatio;
  double TotalDelivery;
  double ViolationStockOut;

  vector<vector<vector<int>>> Route;
  vector<vector<int>> UnallocatedCustomers;
  vector<vector<double>> VehicleLoad;
  vector<vector<double>> DeliveryQuantity;
  vector<vector<double>> InventoryLevel;
  vector<vector<int>> VehicleAllocation;
  vector<vector<int>> VisitOrder;
  vector<double> StockOutPerCustomer;

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