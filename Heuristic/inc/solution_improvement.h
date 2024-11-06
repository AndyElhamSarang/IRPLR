class solution_improvement
{

private:
public:

void LargeNeighbourhoodSearch(input &IRPLR, solution &IRPSolution, HGS &Routing);
void LNS_Repair(input &IRPLR, solution &IRPSolution, HGS &Routing);
void LNS_Destory(input &IRPLR, solution &IRPSolution, HGS &Routing);

double OperatorSwap(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut);
double OperatorSingleRoute(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut);


int opt1();

int opt2();

};