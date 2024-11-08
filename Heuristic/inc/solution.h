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
  vector<vector<double>> DeliveryQuantity;
  vector<vector<double>> InventoryLevel;
  vector<vector<double>> VehicleLoad;

  void OutputCVRP(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  void ReadCVRP_Solution(input &IRPLR, int &day, vector<vector<int>> &RouteForDay);
  int CheckStockOut(input &IRPLR, int &RetailerIndex);
  void print_solution(input &IRPLR);
  void GetLogisticRatio(input &IRPLR);
};