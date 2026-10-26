#include <vector>
#include <string>

class exact
{
 public:
  struct Solution
  {
    struct Delivery
    {
      int customer;
      int vehicle;
      int day;
      double quantity;
    };

    struct RouteEdge
    {
      int from;
      int to;
      int vehicle;
      int day;
      int multiplicity;
    };

    bool hasIncumbent = false;
    std::string cplexStatus = "NOT_RUN";
    int cplexStatusCode = -1;
    bool cplexHasIncumbent = false;
    std::string cplexError;
    double routingCost = 0.0;
    double deliveredQuantity = 0.0;
    double logisticRatio = 0.0;
    double optimalityGap = 0.0;
    std::vector<std::vector<double>> inventoryLevels;
    std::vector<Delivery> deliveries;
    std::vector<RouteEdge> routeEdges;
  };

  struct WarmStart
  {
    std::vector<double> inventory;
    std::vector<double> delivered;
    std::vector<double> visited;
    std::vector<double> edges;
    double totalDelivery = 0.0;
  };

  Solution IRPMODEL(input &IRPLR, double &time_limit, double &memory_limit,
                    const WarmStart *warmStart = nullptr);

  typedef IloArray< IloNumVarArray > NumVarMatrix;
  typedef IloArray< NumVarMatrix >   NumVar3Matrix;
  typedef IloArray<NumVar3Matrix> NumVar4Matrix;
};
