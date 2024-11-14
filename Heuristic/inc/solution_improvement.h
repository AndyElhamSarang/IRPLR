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

int opt1();

int opt2();

};