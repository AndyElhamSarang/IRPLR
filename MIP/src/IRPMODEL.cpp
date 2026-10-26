#include "lib.h"
#include <queue>

namespace {
int inventoryIndex(int day, int node, int nodes) {
    return day * nodes + node;
}

int deliveryIndex(int day, int vehicle, int customer, int vehicles,
                  int customers) {
    return (day * vehicles + vehicle) * customers + customer;
}

int visitIndex(int day, int vehicle, int node, int vehicles, int nodes) {
    return (day * vehicles + vehicle) * nodes + node;
}

int edgePairIndex(int from, int to, int nodes) {
    if (from > to)
        swap(from, to);
    return from * (2 * nodes - from - 1) / 2 + to - from - 1;
}

int edgeIndex(int day, int vehicle, int from, int to, int vehicles,
              int nodes) {
    const int pairs = nodes * (nodes - 1) / 2;
    return (day * vehicles + vehicle) * pairs +
           edgePairIndex(from, to, nodes);
}

class SubtourEliminationCallback : public IloCplex::LazyConstraintCallbackI {
    IloNumVarArray edges_;
    IloNumVarArray visits_;
    int customers_;
    int vehicles_;
    int horizon_;

public:
    SubtourEliminationCallback(IloEnv env, IloNumVarArray edges,
                               IloNumVarArray visits, int customers,
                               int vehicles, int horizon)
        : IloCplex::LazyConstraintCallbackI(env), edges_(edges), visits_(visits),
          customers_(customers), vehicles_(vehicles), horizon_(horizon) {}

    SubtourEliminationCallback(const SubtourEliminationCallback &other)
        : IloCplex::LazyConstraintCallbackI(other), edges_(other.edges_),
          visits_(other.visits_), customers_(other.customers_),
          vehicles_(other.vehicles_), horizon_(other.horizon_) {}

    IloCplex::CallbackI *duplicateCallback() const override {
        return new (getEnv()) SubtourEliminationCallback(*this);
    }

    void main() override {
        const int nodes = customers_ + 1;
        for (int day = 0; day < horizon_; ++day)
            for (int vehicle = 0; vehicle < vehicles_; ++vehicle) {
                vector<int> component(nodes, -1);
                int componentId = 0;
                for (int start = 1; start < nodes; ++start) {
                    if (component[start] != -1)
                        continue;
                    if (getValue(visits_[visitIndex(
                            day, vehicle, start, vehicles_, nodes)]) < 0.5) {
                        component[start] = -2;
                        continue;
                    }

                    queue<int> pending;
                    vector<int> members;
                    bool connectedToDepot = false;
                    component[start] = componentId;
                    pending.push(start);
                    while (!pending.empty()) {
                        const int customerNode = pending.front();
                        pending.pop();
                        members.push_back(customerNode);
                        if (getValue(edges_[edgeIndex(day, vehicle, 0,
                                                      customerNode, vehicles_,
                                                      nodes)]) > 0.5)
                            connectedToDepot = true;

                        for (int other = 1; other < nodes; ++other) {
                            if (other == customerNode ||
                                component[other] != -1 ||
                                getValue(visits_[visitIndex(
                                    day, vehicle, other, vehicles_, nodes)]) <
                                    0.5)
                                continue;
                            if (getValue(edges_[edgeIndex(
                                    day, vehicle, customerNode, other,
                                    vehicles_, nodes)]) > 0.5) {
                                component[other] = componentId;
                                pending.push(other);
                            }
                        }
                    }

                    if (!connectedToDepot && !members.empty()) {
                        IloExpr internalEdges(getEnv());
                        IloExpr visitedCustomers(getEnv());
                        for (size_t i = 0; i < members.size(); ++i) {
                            const int customerNode = members[i];
                            visitedCustomers += visits_[visitIndex(
                                day, vehicle, customerNode, vehicles_, nodes)];
                            for (size_t j = i + 1; j < members.size(); ++j)
                                internalEdges += edges_[edgeIndex(
                                    day, vehicle, customerNode, members[j],
                                    vehicles_, nodes)];
                        }
                        visitedCustomers -= visits_[visitIndex(
                            day, vehicle, members.front(), vehicles_, nodes)];
                        add(internalEdges <= visitedCustomers);
                        internalEdges.end();
                        visitedCustomers.end();
                    }
                    ++componentId;
                }
            }
    }
};
} // namespace

exact::Solution exact::IRPMODEL(input &IRPLR, double &time_limit,
                                double &memory_limit,
                                const WarmStart *warmStart) {
    Solution result;
    IloEnv env;
    try {
        const int customers = IRPLR.NumberOfRetailers;
        const int nodes = customers + 1;
        const int vehicles = IRPLR.NumberOfVehicles;
        const int horizon = IRPLR.TimeHorizon;

        IloModel model(env);
        IloNumVarArray inventory(env);
        IloNumVarArray delivered(env);
        IloNumVarArray visited(env);
        IloNumVarArray edges(env);

        for (int day = 0; day < horizon; ++day)
            for (int node = 0; node < nodes; ++node)
                inventory.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT));

        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int customer = 0; customer < customers; ++customer)
                    delivered.add(
                        IloNumVar(env, 0.0, IloInfinity, ILOFLOAT));

        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int node = 0; node < nodes; ++node)
                    visited.add(IloBoolVar(env));

        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int from = 0; from < nodes; ++from)
                    for (int to = from + 1; to < nodes; ++to)
                        edges.add(IloNumVar(
                            env, 0.0, from == 0 ? 2.0 : 1.0,
                            from == 0 ? ILOINT : ILOBOOL));

        // (1b): supplier inventory balance.
        for (int day = 0; day < horizon; ++day) {
            IloExpr balance(env);
            balance += inventory[inventoryIndex(day, 0, nodes)];
            if (day == 0)
                balance -= IRPLR.SupplierInitialInventory();
            else
                balance -= inventory[inventoryIndex(day - 1, 0, nodes)];
            balance -= IRPLR.SupplierProduction();
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int customer = 0; customer < customers; ++customer)
                    balance += delivered[deliveryIndex(
                        day, vehicle, customer, vehicles, customers)];
            model.add(balance == 0);
            balance.end();
        }

        // (1c)-(1e): customer inventory balance, nonnegative inventory
        // domains, and delivery limits from the previous inventory level.
        for (int customer = 0; customer < customers; ++customer)
            for (int day = 0; day < horizon; ++day) {
                IloExpr balance(env);
                balance += inventory[inventoryIndex(day, customer + 1, nodes)];
                if (day == 0)
                    balance -= IRPLR.CustomerInitialInventory(customer);
                else
                    balance -=
                        inventory[inventoryIndex(day - 1, customer + 1, nodes)];
                balance += IRPLR.CustomerDemand(customer);
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    balance -= delivered[deliveryIndex(
                        day, vehicle, customer, vehicles, customers)];
                model.add(balance == 0);
                balance.end();

                IloExpr capacity(env);
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    capacity += delivered[deliveryIndex(
                        day, vehicle, customer, vehicles, customers)];
                const double previousInventory =
                    day == 0
                        ? IRPLR.CustomerInitialInventory(customer)
                        : 0.0;
                if (day == 0) {
                    model.add(capacity <=
                              IRPLR.CustomerMaximumInventory(customer) -
                                  previousInventory);
                } else {
                    model.add(
                        capacity + inventory[inventoryIndex(
                                       day - 1, customer + 1, nodes)] <=
                        IRPLR.CustomerMaximumInventory(customer));
                }
                capacity.end();
            }

        // (1f): deliveries are allowed only at visited customers.
        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int customer = 0; customer < customers; ++customer)
                    model.add(
                        delivered[deliveryIndex(day, vehicle, customer,
                                                vehicles, customers)] <=
                        IRPLR.CustomerMaximumInventory(customer) *
                            visited[visitIndex(day, vehicle, customer + 1,
                                               vehicles, nodes)]);

        // (1g): vehicle capacity.
        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle) {
                IloExpr load(env);
                for (int customer = 0; customer < customers; ++customer)
                    load += delivered[deliveryIndex(
                        day, vehicle, customer, vehicles, customers)];
                model.add(load <= IRPLR.Vehicle.capacity *
                                       visited[visitIndex(day, vehicle, 0,
                                                          vehicles, nodes)]);
                load.end();
            }

        // (1h): each customer can be served by at most one vehicle per day.
        for (int day = 0; day < horizon; ++day)
            for (int customer = 0; customer < customers; ++customer) {
                IloExpr numberOfVisits(env);
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    numberOfVisits += visited[visitIndex(
                        day, vehicle, customer + 1, vehicles, nodes)];
                model.add(numberOfVisits <= 1);
                numberOfVisits.end();
            }

        // (1i): two incident route edges for every visited node.
        for (int day = 0; day < horizon; ++day)
            for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                for (int node = 0; node < nodes; ++node) {
                    IloExpr degree(env);
                    for (int other = 0; other < nodes; ++other)
                        if (other != node)
                            degree += edges[edgeIndex(
                                day, vehicle, node, other, vehicles, nodes)];
                    model.add(degree ==
                              2 * visited[visitIndex(day, vehicle, node,
                                                     vehicles, nodes)]);
                    degree.end();
                }

        // (1j) is separated lazily at integer solutions as subtour cuts.
        // Variable domains implement (1d), (1k)-(1n): inventory/delivery
        // quantities are nonnegative, z is binary, and depot edges are
        // integer in [0, 2] while customer edges are binary.

        if (warmStart != nullptr) {
            const int expectedInventory = horizon * nodes;
            const int expectedDeliveries = horizon * vehicles * customers;
            const int expectedVisits = horizon * vehicles * nodes;
            const int expectedEdges =
                horizon * vehicles * nodes * (nodes - 1) / 2;
            if (warmStart->inventory.size() !=
                    static_cast<size_t>(expectedInventory) ||
                warmStart->delivered.size() !=
                    static_cast<size_t>(expectedDeliveries) ||
                warmStart->visited.size() !=
                    static_cast<size_t>(expectedVisits) ||
                warmStart->edges.size() !=
                    static_cast<size_t>(expectedEdges))
                throw invalid_argument("JSON warm-start variable dimensions do not match the instance");

            const IloNumVarArray *variableGroups[] = {
                &inventory, &delivered, &visited, &edges};
            const vector<double> *valueGroups[] = {
                &warmStart->inventory, &warmStart->delivered,
                &warmStart->visited, &warmStart->edges};
            for (size_t group = 0; group < 4; ++group)
                for (int index = 0; index < variableGroups[group]->getSize();
                     ++index)
                    model.add((*variableGroups[group])[index] ==
                              (*valueGroups[group])[index]);

            IloExpr totalDelivery(env);
            for (int index = 0; index < delivered.getSize(); ++index)
                totalDelivery += delivered[index];
            model.add(totalDelivery == warmStart->totalDelivery);
            totalDelivery.end();

            IloExpr routingCost(env);
            double warmStartRoutingCost = 0.0;
            for (int day = 0; day < horizon; ++day)
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    for (int from = 0; from < nodes; ++from)
                        for (int to = from + 1; to < nodes; ++to) {
                            const int index = edgeIndex(
                                day, vehicle, from, to, vehicles, nodes);
                            routingCost += IRPLR.Distance[from][to] *
                                           edges[index];
                            warmStartRoutingCost +=
                                IRPLR.Distance[from][to] *
                                warmStart->edges[index];
                        }
            model.add(routingCost == warmStartRoutingCost);
            routingCost.end();
        }

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, time_limit);
        cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory,
                       memory_limit / (1024.0 * 1024.0));
        if (warmStart != nullptr) {
            IloNumVarArray startVariables(env);
            IloNumArray startValues(env);
            const IloNumVarArray *variableGroups[] = {
                &inventory, &delivered, &visited, &edges};
            const vector<double> *valueGroups[] = {
                &warmStart->inventory, &warmStart->delivered,
                &warmStart->visited, &warmStart->edges};
            for (size_t group = 0; group < 4; ++group)
                for (int index = 0; index < variableGroups[group]->getSize();
                     ++index) {
                    startVariables.add((*variableGroups[group])[index]);
                    startValues.add((*valueGroups[group])[index]);
                }
            cplex.addMIPStart(startVariables, startValues,
                              IloCplex::MIPStartCheckFeas);
            env.out() << "Fixed model variables to the JSON warm start and "
                         "constrained its routing cost."
                      << endl;
            startVariables.end();
            startValues.end();
        }
        cplex.use(new (env) SubtourEliminationCallback(
            env, edges, visited, customers, vehicles, horizon));

        cplex.exportModel("irp.lp");
        cplex.solve();
        {
            ostringstream status;
            const IloCplex::CplexStatus cplexStatus =
                cplex.getCplexStatus();
            result.cplexStatusCode = static_cast<int>(cplexStatus);
            status << cplexStatus;
            result.cplexStatus = status.str();
        }
        result.cplexHasIncumbent = cplex.isPrimalFeasible();
        result.hasIncumbent = result.cplexHasIncumbent;
        if (result.hasIncumbent) {
            result.inventoryLevels.assign(
                nodes, vector<double>(horizon, 0.0));
            for (int day = 0; day < horizon; ++day)
                for (int node = 0; node < nodes; ++node)
                    result.inventoryLevels[node][day] =
                        cplex.getValue(
                            inventory[inventoryIndex(day, node, nodes)]);

            for (int day = 0; day < horizon; ++day)
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    for (int customer = 0; customer < customers; ++customer) {
                        const double quantity = cplex.getValue(
                            delivered[deliveryIndex(day, vehicle, customer,
                                                    vehicles, customers)]);
                        result.deliveredQuantity += quantity;
                        if (quantity > 1e-6)
                            result.deliveries.push_back(
                                {customer, vehicle, day, quantity});
                    }

            for (int day = 0; day < horizon; ++day)
                for (int vehicle = 0; vehicle < vehicles; ++vehicle)
                    for (int from = 0; from < nodes; ++from)
                        for (int to = from + 1; to < nodes; ++to) {
                            const int multiplicity = static_cast<int>(round(
                                cplex.getValue(edges[edgeIndex(
                                    day, vehicle, from, to, vehicles, nodes)])));
                            if (multiplicity > 0) {
                                result.routingCost +=
                                    IRPLR.Distance[from][to] * multiplicity;
                                result.routeEdges.push_back(
                                    {from, to, vehicle, day, multiplicity});
                            }
                        }
            if (result.deliveredQuantity > 1e-9)
                result.logisticRatio =
                    result.routingCost / result.deliveredQuantity;
            env.out() << "Feasibility status = " << cplex.getStatus() << endl;
        } else {
            env.out() << "No feasible solution found; status = "
                      << cplex.getCplexStatus() << endl;
        }
    } catch (IloException &exception) {
        result.cplexStatus = "CPLEX_ERROR";
        result.cplexError = exception.getMessage();
        cerr << "CPLEX error: " << exception << endl;
    } catch (const exception &error) {
        result.cplexStatus = "MODEL_ERROR";
        result.cplexError = error.what();
        cerr << "Model error: " << error.what() << endl;
    } catch (...) {
        result.cplexStatus = "UNKNOWN_ERROR";
        result.cplexError = "Unknown exception while building or solving the IRP model";
        cerr << "Unknown exception while building or solving the IRP model"
             << endl;
    }
    env.end();
    return result;
}
