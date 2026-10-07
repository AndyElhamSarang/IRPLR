#ifndef IRPLR_MODEL_H
#define IRPLR_MODEL_H

#include <ilcplex/ilocplex.h>
#include "input.h"
#include <vector>

ILOSTLBEGIN

class IRPLRModel {
public:
  struct Solution {
    struct Delivery {
      int customer;
      int vehicle;
      int day;
      double quantity;
    };

    struct RouteEdge {
      int from;
      int to;
      int vehicle;
      int day;
      int multiplicity;
    };

    double routingCost;
    double deliveredQuantity;
    double logisticRatio;
    double optimalityGap;
    IloAlgorithm::Status status;
    std::vector<std::vector<double>> inventoryLevels;
    std::vector<Delivery> deliveries;
    std::vector<RouteEdge> routeEdges;
  };

  IRPLRModel(IloEnv env, const input &instance);
  Solution minimizeRouting(double minimumDelivered = 0.0);
  Solution maximizeDelivered(double maximumRoutingCost);

private:
  IloEnv env_;
  const input &instance_;
  IloModel model_;
  IloNumVarArray inventory_;
  IloNumVarArray delivered_;
  IloNumVarArray visited_;
  IloNumVarArray edges_;

  IloNumVar inventory(int node, int day);
  IloNumVar delivered(int customer, int vehicle, int day);
  IloNumVar visited(int node, int vehicle, int day);
  IloNumVar edge(int from, int to, int vehicle, int day);
  IloExpr routingCost();
  IloExpr deliveredQuantity();
  void buildVariables();
  void buildConstraints();
  void addSubsetConstraints();
  Solution solve(bool minimizeRouting, double bound);
};

#endif
