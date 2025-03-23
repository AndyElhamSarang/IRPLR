class solution_improvement
{

private:
public:

void LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory);
void LNS_Repair(input &IRPLR, solution &IRPSolution, HGS &Routing);
void LNS_Destory(input &IRPLR, solution &IRPSolution, HGS &Routing);

int OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut,preprocessing &memory,
                 int &min_remove_length, int &max_remove_length, int &min_insert_length, int &max_insert_length);

int OperatorSwapWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut,preprocessing &memory, 
                              int &min_remove_length, int &max_remove_length, int &min_insert_length, int &max_insert_length);

int OperatorInsert(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory);
int OperatorRemove(input &IRPLR, solution &IRPSolution,  double &PenaltyForStockOut, preprocessing &memory);
double OperatorBalancing(input &IRPLR, preprocessing &memory,
vector<vector<vector<int>>> &Route, 
vector<vector<int>> &UnallocatedCustomers,
vector<vector<double>> &VehicleLoad,
vector<vector<double>> &DeliveryQuantity,
vector<vector<double>> &InventoryLevel,
vector<vector<int>> &VehicleAllocation,
vector<vector<int>> &VisitOrder,
int &CountingInfeasibleCase,
int & FeasibleRebalanceOrNot);

int OperatorCheapestInsertion(input &IRPLR, vector<int> &route, int &CustomerToReinsert, double &PenaltyForStockOut, double &CurrentTransportationCost,preprocessing &memory);
int OperatorRepair(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut);

void PrintTempSolution(input &IRPLR, 
                       vector<vector<vector<int>>> &Route,
                       vector<vector<int>> &UnallocatedCustomers,
                       vector<vector<double>> &VehicleLoad,
                       vector<vector<double>> &DeliveryQuantity,
                       vector<vector<double>> &InventoryLevel,
                       vector<vector<int>> &VehicleAllocation,
                       vector<vector<int>> &VisitOrder);

void GetTempLogisticRatio(input &IRPLR, vector<vector<vector<int>>> &Route,
                            vector<vector<double>> &DeliveryQuantity,
                            vector<vector<double>> &InventoryLevel,
                            double &TotalTransportationCost,
                            double &TotalDelivery,
                            double &LogisticRatio,
                            double &ViolationStockOut);
int opt1();

int opt2();

};