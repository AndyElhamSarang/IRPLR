class solution_improvement
{

private:
public:

void LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory);
void LNS_Repair(input &IRPLR, solution &IRPSolution, HGS &Routing);
void LNS_Destory(input &IRPLR, solution &IRPSolution, HGS &Routing);

double OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut,preprocessing &memory);
double OperatorInsert(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory);
double OperatorRemove(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory);
double OperatorBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory);

int OperatorCheapestInsertion(input &IRPLR, vector<int> &route, int &CustomerToReinsert, double &PenaltyForStockOut, double &CurrentTransportationCost,preprocessing &memory);
double OperatorRepair(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut);

void PrintTempSolution(input &IRPLR, vector<vector<vector<int>>> &Route,vector<vector<int>> &UnallocatedCustomers,vector<vector<double>> &VehicleLoad,vector<vector<double>> &DeliveryQuantity,vector<vector<double>> &InventoryLevel,vector<vector<int>> &VehicleAllocation);

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