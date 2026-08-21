// #include "irplr_model.h"
// #include <algorithm>
// #include <stdexcept>

// namespace {
// int invIndex(int node, int day, int horizon) { return node * horizon + day; }
// int qIndex(int customer, int vehicle, int day, int vehicles, int horizon) {
//   return (customer * vehicles + vehicle) * horizon + day;
// }
// int zIndex(int node, int vehicle, int day, int vehicles, int horizon) {
//   return (node * vehicles + vehicle) * horizon + day;
// }
// int pairIndex(int from, int to, int nodes) {
//   if (from > to) std::swap(from, to);
//   return from * (2 * nodes - from - 1) / 2 + to - from - 1;
// }
// int yIndex(int from, int to, int vehicle, int day, int nodes, int vehicles, int horizon) {
//   return (pairIndex(from, to, nodes) * vehicles + vehicle) * horizon + day;
// }
// }

// IRPLRModel::IRPLRModel(IloEnv env, const input &instance)
//     : env_(env), instance_(instance), model_(env), inventory_(env), delivered_(env),
//       visited_(env), edges_(env) {
//   buildVariables();
//   buildConstraints();
// }

// IloNumVar IRPLRModel::inventory(int node, int day) {
//   return inventory_[invIndex(node, day, instance_.TimeHorizon)];
// }
// IloNumVar IRPLRModel::delivered(int customer, int vehicle, int day) {
//   return delivered_[qIndex(customer, vehicle, day, instance_.NumberOfVehicles,
//                             instance_.TimeHorizon)];
// }
// IloNumVar IRPLRModel::visited(int node, int vehicle, int day) {
//   return visited_[zIndex(node, vehicle, day, instance_.NumberOfVehicles,
//                           instance_.TimeHorizon)];
// }
// IloNumVar IRPLRModel::edge(int from, int to, int vehicle, int day) {
//   const int nodes = instance_.NumberOfRetailers + 1;
//   return edges_[yIndex(from, to, vehicle, day, nodes, instance_.NumberOfVehicles,
//                         instance_.TimeHorizon)];
// }

// void IRPLRModel::buildVariables() {
//   const int customers = instance_.NumberOfRetailers;
//   const int nodes = customers + 1;
//   const int horizon = instance_.TimeHorizon;
//   const int vehicles = instance_.NumberOfVehicles;

//   for (int node = 0; node < nodes; ++node)
//     for (int day = 0; day < horizon; ++day)
//       inventory_.add(IloNumVar(env_, 0.0, IloInfinity, ILOFLOAT));
//   for (int customer = 0; customer < customers; ++customer)
//     for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//       for (int day = 0; day < horizon; ++day)
//         delivered_.add(IloNumVar(env_, 0.0, IloInfinity, ILOFLOAT));
//   for (int node = 0; node < nodes; ++node)
//     for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//       for (int day = 0; day < horizon; ++day)
//         visited_.add(IloBoolVar(env_));
//   for (int from = 0; from < nodes; ++from)
//     for (int to = from + 1; to < nodes; ++to)
//       for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//         for (int day = 0; day < horizon; ++day)
//           edges_.add(IloNumVar(env_, 0.0, from == 0 ? 2.0 : 1.0,
//                                from == 0 ? ILOINT : ILOBOOL));
// }

// IloExpr IRPLRModel::routingCost() {
//   IloExpr expression(env_);
//   for (int from = 0; from <= instance_.NumberOfRetailers; ++from)
//     for (int to = from + 1; to <= instance_.NumberOfRetailers; ++to)
//       for (int vehicle = 0; vehicle < instance_.NumberOfVehicles; ++vehicle)
//         for (int day = 0; day < instance_.TimeHorizon; ++day)
//           expression += instance_.Distance[from][to] * edge(from, to, vehicle, day);
//   return expression;
// }

// IloExpr IRPLRModel::deliveredQuantity() {
//   IloExpr expression(env_);
//   for (int customer = 0; customer < instance_.NumberOfRetailers; ++customer)
//     for (int vehicle = 0; vehicle < instance_.NumberOfVehicles; ++vehicle)
//       for (int day = 0; day < instance_.TimeHorizon; ++day)
//         expression += delivered(customer, vehicle, day);
//   return expression;
// }

// void IRPLRModel::buildConstraints() {
//   const int customers = instance_.NumberOfRetailers;
//   const int horizon = instance_.TimeHorizon;
//   const int vehicles = instance_.NumberOfVehicles;

//   for (int day = 0; day < horizon; ++day) {
//     IloExpr balance(env_);
//     balance += inventory(0, day) - instance_.SupplierProduction();
//     if (day > 0) balance -= inventory(0, day - 1);
//     else balance -= instance_.SupplierInitialInventory();
//     for (int customer = 0; customer < customers; ++customer)
//       for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//         balance += delivered(customer, vehicle, day);
//     model_.add(balance == 0);
//     balance.end();
//   }
//   for (int customer = 0; customer < customers; ++customer)
//     for (int day = 0; day < horizon; ++day) {
//       IloExpr balance(env_);
//       balance += inventory(customer + 1, day) + instance_.CustomerDemand(customer);
//       if (day > 0) balance -= inventory(customer + 1, day - 1);
//       else balance -= instance_.CustomerInitialInventory(customer);
//       for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//         balance -= delivered(customer, vehicle, day);
//       model_.add(balance == 0);
//       balance.end();

//       IloExpr quantity(env_);
//       for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//         quantity += delivered(customer, vehicle, day);
//       if (day == 0)
//         model_.add(quantity <= instance_.CustomerMaximumInventory(customer) -
//                    instance_.CustomerInitialInventory(customer));
//       else
//         model_.add(quantity <= instance_.CustomerMaximumInventory(customer) -
//                    inventory(customer + 1, day - 1));
//       quantity.end();
//     }

//   for (int customer = 0; customer < customers; ++customer)
//     for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//       for (int day = 0; day < horizon; ++day)
//         model_.add(delivered(customer, vehicle, day) <=
//                    instance_.CustomerMaximumInventory(customer) *
//                    visited(customer + 1, vehicle, day));
//   for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//     for (int day = 0; day < horizon; ++day) {
//       IloExpr capacity(env_);
//       for (int customer = 0; customer < customers; ++customer)
//         capacity += delivered(customer, vehicle, day);
//       model_.add(capacity <= instance_.Vehicle.capacity * visited(0, vehicle, day));
//       capacity.end();
//     }
//   for (int customer = 0; customer < customers; ++customer)
//     for (int day = 0; day < horizon; ++day) {
//       IloExpr visits(env_);
//       for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//         visits += visited(customer + 1, vehicle, day);
//       model_.add(visits <= 1);
//       visits.end();
//     }
//   for (int node = 0; node <= customers; ++node)
//     for (int vehicle = 0; vehicle < vehicles; ++vehicle)
//       for (int day = 0; day < horizon; ++day) {
//         IloExpr degree(env_);
//         for (int other = 0; other <= customers; ++other)
//           if (other != node) degree += edge(node, other, vehicle, day);
//         model_.add(degree == 2 * visited(node, vehicle, day));
//         degree.end();
//       }
//   addSubsetConstraints();
// }

// void IRPLRModel::addSubsetConstraints() {
//   const int customers = instance_.NumberOfRetailers;
//   if (customers >= 25)
//     throw std::runtime_error("Explicit subset subtour constraints require fewer than 25 customers");
//   for (int mask = 1; mask < (1 << customers); ++mask)
//     for (int selected = 0; selected < customers; ++selected)
//       if (mask & (1 << selected))
//         for (int vehicle = 0; vehicle < instance_.NumberOfVehicles; ++vehicle)
//           for (int day = 0; day < instance_.TimeHorizon; ++day) {
//             IloExpr lhs(env_), rhs(env_);
//             for (int from = 0; from < customers; ++from)
//               if (mask & (1 << from))
//                 for (int to = from + 1; to < customers; ++to)
//                   if (mask & (1 << to)) lhs += edge(from + 1, to + 1, vehicle, day);
//             for (int customer = 0; customer < customers; ++customer)
//               if (mask & (1 << customer)) rhs += visited(customer + 1, vehicle, day);
//             rhs -= visited(selected + 1, vehicle, day);
//             model_.add(lhs <= rhs);
//             lhs.end();
//             rhs.end();
//           }
// }

// IRPLRModel::Solution IRPLRModel::solve(bool minimize, double bound) {
//   IloExpr objective = minimize ? routingCost() : deliveredQuantity();
//   model_.add(minimize ? IloMinimize(env_, objective) : IloMaximize(env_, objective));
//   if (minimize && bound > 0) {
//     IloExpr quantity = deliveredQuantity();
//     model_.add(quantity >= bound);
//     quantity.end();
//   }
//   if (!minimize) {
//     IloExpr cost = routingCost();
//     model_.add(cost <= bound);
//     cost.end();
//   }
//   objective.end();
//   IloCplex cplex(model_);

//   Solution result = {0.0, 0.0, IloAlgorithm::Unknown};
//   if (cplex.solve()) {
//     result.status = cplex.getStatus();
//     IloExpr cost = routingCost();
//     IloExpr quantity = deliveredQuantity();
//     result.routingCost = cplex.getValue(cost);
//     result.deliveredQuantity = cplex.getValue(quantity);
//     cost.end();
//     quantity.end();
//     result.logisticRatio = result.deliveredQuantity > 0
//                                ? result.routingCost / result.deliveredQuantity
//                                : IloInfinity;
//   }
//   return result;
// }

// IRPLRModel::Solution IRPLRModel::minimizeRouting(double minimumDelivered) {
//   return solve(true, minimumDelivered);
// }

// IRPLRModel::Solution IRPLRModel::maximizeDelivered(double maximumRoutingCost) {
//   return solve(false, maximumRoutingCost);
// }
