#ifndef IRPLR_MODEL_H
#define IRPLR_MODEL_H

#include <ilcplex/ilocplex.h>
#include "input.h"

ILOSTLBEGIN

class IRPLRModel {
public:
  struct Solution {
    double routingCost;
    double deliveredQuantity;
    double logisticRatio;
    IloAlgorithm::Status status;
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
