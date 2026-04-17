class solution_improvement
{

private:
public:

void LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory);
void IteratedLocalSearch(input &IRPLR, solution &IRPSolution, HGS &Routing, preprocessing &memory, solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s,solution &IRPSolution60s);
void LNS_Repair(input &IRPLR, solution &IRPSolution, HGS &Routing);
void LNS_Destory(input &IRPLR, solution &IRPSolution, HGS &Routing);

double DeliveryMax(
    double &VehicleCapacity, 
    double &VehicleLoad, 
    double &InventoryMax, 
    double &CustomerDemand,
    double &PreviousInventoryLevel
);
double Calculate_la_relax_objv(
    double &logistic_ratio, 
    double &stockout_penalty, 
    double &stockout
);
void AdjustQuantityAndInventoryLevel(
    double &begining_inventory, 
    int &day, 
    int &vehicle,
    vector<double> &DeliveryQuantity,
    vector<double> &InventoryLevel, 
    vector<vector<double>> &VehicleLoad, 
    vector<vector<int>> &VehicleAllocation,
    double &ChangeInTotalQuantity, 
    double &NewStockOut, 
    int &customer_index, 
    input &IRPLR);
int OperatorPureSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut,preprocessing &memory);

int OperatorSwapRemoveInsert(
    input &IRPLR, 
    solution &IRPSolution, 
    double &PenaltyForStockOut,
    preprocessing &memory,
    set<vector<int>> &SwapRemoveInsertPair,
    int &min_remove_length, 
    int &max_remove_length, 
    int &min_insert_length, 
    int &max_insert_length
);

int OperatorSwapTwoRoutesOnSingleDay(
    input &IRPLR, 
    solution &IRPSolution, 
    double &PenaltyForStockOut, 
    preprocessing &memory,
    set<vector<int>> &SwapTwoRoutesOnSingleDayPair,
    set<vector<int>> SwapTwoRoutesOnSingleDayPairToReconsider,
    int &min_remove_length, 
    int &max_remove_length,
    int &min_insert_length,
    int &max_insert_length
);

int OperatorSwapRemoveInsertWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut,preprocessing &memory, 
                              int &min_remove_length, int &max_remove_length, int &min_insert_length, int &max_insert_length);

int OperatorInterSwap(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory,
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

int OperatorIntra(input &IRPLR, vector<int> &route, int &day, int &vehicle, int &MinToReinsert, int &MaxToReinsert, preprocessing &memory, double &CurrentTransportationCost);

void OperatorDisturb(input &IRPLR, solution &GlobalBest, solution &IRPSolution, int &DisturbanceCounter, int &MaxDisturbance);

int OperatorCheapestInsertion(input &IRPLR, vector<int> &route, int &day, int &vehicle, int &CustomerToReinsert, double &PenaltyForStockOut, double &CurrentTransportationCost,preprocessing &memory);

int OperatorRepair(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut);

int OperatorTransfer(
    input &IRPLR,
    solution &IRPSolution,
    double &PenaltyForStockOut,
    vector<vector<int>> &TransferDetails,
    preprocessing &memory);

int LocalSearch(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory,solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s,solution &IRPSolution60s);


int ImprovedLocalSearch(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, preprocessing &memory,solution &GlobalBest, solution &FirstImprovementSolution, solution &IRPSolution30s,solution &IRPSolution60s, int &DisturbanceCounter, bool &RunHGSAtEnd);

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

};