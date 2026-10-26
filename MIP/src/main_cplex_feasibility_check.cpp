#include "lib.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmath>

int printout_inputdata = 0;
int printout_initialSchedule = 0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;
double power = 2.0;
ofstream Table;
string MachineDirectory;
string JSONDirectory;
int OutputResults = 0;

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

void requireMatchingValue(double solutionValue, double instanceValue,
                          const string &field, const string &path) {
  if (!isfinite(solutionValue) || fabs(solutionValue - instanceValue) > 1e-6)
    throw runtime_error("Solution field '" + field +
                        "' does not match the loaded instance: " + path);
}

string dataRootFolder(const string &instanceDirectory) {
  string normalizedDirectory = instanceDirectory;
  replace(normalizedDirectory.begin(), normalizedDirectory.end(), '\\', '/');
  const string dataMarker = "Data/";
  const size_t dataPosition = normalizedDirectory.find(dataMarker);
  if (dataPosition == string::npos)
    throw runtime_error("Instance directory does not identify a Data root: " +
                        instanceDirectory);

  const size_t rootStart = dataPosition + dataMarker.size();
  const size_t rootEnd = normalizedDirectory.find('/', rootStart);
  const string root = normalizedDirectory.substr(
      rootStart, rootEnd == string::npos ? string::npos : rootEnd - rootStart);
  if (root.empty())
    throw runtime_error("Instance directory has an empty data root: " +
                        instanceDirectory);
  return root;
}

exact::WarmStart readWarmStart(const input &instance,
                               const string &instanceDirectory) {
  string instanceStem = instance.InstanceName;
  const size_t extension = instanceStem.rfind(".dat");
  if (extension != string::npos)
    instanceStem.erase(extension);
  const string rootFolder = dataRootFolder(instanceDirectory);
  const string path = JSONDirectory + rootFolder + "_" + instanceStem +
                      "_global_best.json";

  boost::property_tree::ptree root;
  boost::property_tree::read_json(path, root);
  const auto &metadata = root.get_child("metadata");
  if (metadata.get<int>("time_horizon") != instance.TimeHorizon ||
      metadata.get<int>("num_vehicles") != instance.NumberOfVehicles ||
      metadata.get<int>("num_retailers") != instance.NumberOfRetailers + 1)
    throw runtime_error("Solution metadata does not match instance: " + path);
  requireMatchingValue(metadata.get<double>("vehicle_capacity"),
                       instance.Vehicle.capacity, "vehicle_capacity", path);
  requireMatchingValue(metadata.get<double>("depot_x"),
                       instance.SupplierXCoordinate(), "depot_x", path);
  requireMatchingValue(metadata.get<double>("depot_y"),
                       instance.SupplierYCoordinate(), "depot_y", path);

  const auto &retailerData = root.get_child("retailers");
  if (retailerData.size() !=
      static_cast<size_t>(instance.NumberOfRetailers))
    throw runtime_error("Solution retailer count does not match instance: " +
                        path);
  vector<bool> retailerMetadataRead(instance.NumberOfRetailers, false);
  for (const auto &retailer : retailerData) {
    const int customer = retailer.second.get<int>("id");
    if (customer < 0 || customer >= instance.NumberOfRetailers ||
        retailerMetadataRead[customer])
      throw runtime_error("Solution has duplicate or invalid retailer metadata: " +
                          path);
    retailerMetadataRead[customer] = true;
    const string fieldPrefix = "retailer " + to_string(customer) + " ";
    requireMatchingValue(retailer.second.get<double>("x_coord"),
                         instance.CustomerXCoordinate(customer),
                         fieldPrefix + "x_coord", path);
    requireMatchingValue(retailer.second.get<double>("y_coord"),
                         instance.CustomerYCoordinate(customer),
                         fieldPrefix + "y_coord", path);
    requireMatchingValue(retailer.second.get<double>("inventory_begin"),
                         instance.CustomerInitialInventory(customer),
                         fieldPrefix + "inventory_begin", path);
    requireMatchingValue(retailer.second.get<double>("inventory_max"),
                         instance.CustomerMaximumInventory(customer),
                         fieldPrefix + "inventory_max", path);
    requireMatchingValue(retailer.second.get<double>("inventory_min"),
                         instance.CustomerMinimumInventory(customer),
                         fieldPrefix + "inventory_min", path);
    requireMatchingValue(retailer.second.get<double>("demand"),
                         instance.CustomerDemand(customer),
                         fieldPrefix + "demand", path);
  }

  const int customers = instance.NumberOfRetailers;
  const int nodes = customers + 1;
  const int vehicles = instance.NumberOfVehicles;
  const int horizon = instance.TimeHorizon;
  exact::WarmStart start;
  start.inventory.assign(horizon * nodes, 0.0);
  start.delivered.assign(horizon * vehicles * customers, 0.0);
  start.visited.assign(horizon * vehicles * nodes, 0.0);
  start.edges.assign(horizon * vehicles * nodes * (nodes - 1) / 2, 0.0);

  vector<bool> vehicleScheduled(horizon * vehicles, false);
  vector<bool> customerScheduled(horizon * customers, false);
  vector<double> customerDeliveries(horizon * customers, 0.0);
  for (const auto &period : root.get_child("schedule")) {
    const int day = period.second.get<int>("time_period");
    if (day < 0 || day >= horizon)
      throw runtime_error("Solution contains an invalid time period: " + path);

    for (const auto &vehicleRecord : period.second.get_child("vehicles")) {
      const int vehicle = vehicleRecord.second.get<int>("vehicle_id");
      if (vehicle < 0 || vehicle >= vehicles ||
          vehicleScheduled[day * vehicles + vehicle])
        throw runtime_error("Solution contains a duplicate or invalid vehicle: " +
                            path);
      vehicleScheduled[day * vehicles + vehicle] = true;

      vector<int> route;
      for (const auto &customer : vehicleRecord.second.get_child("route"))
        route.push_back(customer.second.get_value<int>());
      vector<double> quantities;
      for (const auto &quantity :
           vehicleRecord.second.get_child("deliveries"))
        quantities.push_back(quantity.second.get_value<double>());
      if (route.size() != quantities.size())
        throw runtime_error("Route and delivery counts differ in solution: " +
                            path);

      for (size_t position = 0; position < route.size(); ++position) {
        const int customer = route[position];
        const double quantity = quantities[position];
        if (customer < 0 || customer >= customers || !isfinite(quantity) ||
            quantity < 0.0 ||
            customerScheduled[day * customers + customer])
          throw runtime_error("Solution contains an invalid or split customer visit: " +
                              path);
        customerScheduled[day * customers + customer] = true;
        customerDeliveries[day * customers + customer] += quantity;
        start.delivered[deliveryIndex(day, vehicle, customer, vehicles,
                                      customers)] = quantity;
        start.visited[visitIndex(day, vehicle, customer + 1, vehicles, nodes)] =
            1.0;
      }

      if (!route.empty()) {
        start.visited[visitIndex(day, vehicle, 0, vehicles, nodes)] = 1.0;
        ++start.edges[edgeIndex(day, vehicle, 0, route.front() + 1, vehicles,
                                nodes)];
        for (size_t position = 1; position < route.size(); ++position)
          ++start.edges[edgeIndex(day, vehicle, route[position - 1] + 1,
                                  route[position] + 1, vehicles, nodes)];
        ++start.edges[edgeIndex(day, vehicle, route.back() + 1, 0, vehicles,
                                nodes)];
      }
    }
  }

  for (int scheduled = 0; scheduled < horizon * vehicles; ++scheduled)
    if (!vehicleScheduled[scheduled])
      throw runtime_error("Solution omits a day/vehicle schedule: " + path);

  double scheduleTotalDelivery = 0.0;
  for (double delivery : start.delivered)
    scheduleTotalDelivery += delivery;
  const double reportedTotalDelivery =
      root.get<double>("metrics.total_delivery");
  if (!isfinite(reportedTotalDelivery) ||
      fabs(reportedTotalDelivery - scheduleTotalDelivery) > 1e-6)
    throw runtime_error(
        "Solution metrics.total_delivery does not match schedule deliveries: " +
        path);
  start.totalDelivery = reportedTotalDelivery;

  vector<bool> retailerRead(customers, false);
  vector<vector<double>> reportedInventory(customers,
                                           vector<double>(horizon, 0.0));
  for (const auto &retailer : root.get_child("inventory_and_delivery")) {
    const int customer = retailer.second.get<int>("retailer_id");
    if (customer < 0 || customer >= customers || retailerRead[customer])
      throw runtime_error("Solution contains duplicate or invalid retailer data: " +
                          path);
    retailerRead[customer] = true;
    vector<bool> dayRead(horizon, false);
    for (const auto &timeline : retailer.second.get_child("timeline")) {
      const int day = timeline.second.get<int>("time_period");
      if (day < 0 || day >= horizon || dayRead[day])
        throw runtime_error("Solution contains duplicate or invalid inventory data: " +
                            path);
      dayRead[day] = true;
      const double delivery = timeline.second.get<double>("delivery_quantity");
      const double inventory = timeline.second.get<double>("inventory_level");
      if (fabs(delivery - customerDeliveries[day * customers + customer]) >
              1e-6 ||
          !isfinite(inventory))
        throw runtime_error("Inventory timeline disagrees with delivery schedule: " +
                            path);
      reportedInventory[customer][day] = inventory;
    }
    for (bool read : dayRead)
      if (!read)
        throw runtime_error("Solution omits a customer inventory period: " +
                            path);
  }
  for (bool read : retailerRead)
    if (!read)
      throw runtime_error("Solution omits retailer inventory data: " + path);

  double depotInventory = instance.SupplierInitialInventory();
  for (int day = 0; day < horizon; ++day) {
    double totalDelivery = 0.0;
    for (int vehicle = 0; vehicle < vehicles; ++vehicle) {
      double load = 0.0;
      for (int customer = 0; customer < customers; ++customer)
        load += start.delivered[deliveryIndex(day, vehicle, customer,
                                             vehicles, customers)];
      if (load > instance.Vehicle.capacity + 1e-6)
        throw runtime_error("Warm start exceeds vehicle capacity: " + path);
      totalDelivery += load;
    }
    depotInventory += instance.SupplierProduction() - totalDelivery;
    if (depotInventory < -1e-6)
      throw runtime_error("Warm start causes supplier stockout: " + path);
    start.inventory[inventoryIndex(day, 0, nodes)] = depotInventory;

    for (int customer = 0; customer < customers; ++customer) {
      double previousInventory =
          day == 0 ? instance.CustomerInitialInventory(customer)
                   : start.inventory[inventoryIndex(day - 1, customer + 1,
                                                     nodes)];
      const double delivery =
          customerDeliveries[day * customers + customer];
      if (delivery > instance.CustomerMaximumInventory(customer) -
                         previousInventory +
                     1e-6)
        throw runtime_error("Warm start exceeds retailer inventory capacity: " +
                            path);
      const double inventory = previousInventory -
                               instance.CustomerDemand(customer) + delivery;
      if (inventory < -1e-6 ||
          fabs(inventory - reportedInventory[customer][day]) > 1e-6)
        throw runtime_error("Warm start violates customer inventory balance: " +
                            path);
      start.inventory[inventoryIndex(day, customer + 1, nodes)] = inventory;
    }
  }

  cout << "Using matching solution file: " << path << endl;
  return start;
}

exact::Solution solutionFromWarmStart(const input &instance,
                                     const exact::WarmStart &warmStart) {
  exact::Solution solution;
  solution.hasIncumbent = true;
  const int customers = instance.NumberOfRetailers;
  const int nodes = customers + 1;
  const int vehicles = instance.NumberOfVehicles;
  const int horizon = instance.TimeHorizon;
  solution.inventoryLevels.assign(nodes, vector<double>(horizon, 0.0));

  for (int day = 0; day < horizon; ++day)
    for (int node = 0; node < nodes; ++node)
      solution.inventoryLevels[node][day] =
          warmStart.inventory[inventoryIndex(day, node, nodes)];

  for (int day = 0; day < horizon; ++day)
    for (int vehicle = 0; vehicle < vehicles; ++vehicle)
      for (int customer = 0; customer < customers; ++customer) {
        const double quantity =
            warmStart.delivered[deliveryIndex(day, vehicle, customer,
                                              vehicles, customers)];
        solution.deliveredQuantity += quantity;
        if (quantity > 1e-6)
          solution.deliveries.push_back({customer, vehicle, day, quantity});
      }

  for (int day = 0; day < horizon; ++day)
    for (int vehicle = 0; vehicle < vehicles; ++vehicle)
      for (int from = 0; from < nodes; ++from)
        for (int to = from + 1; to < nodes; ++to) {
          const int multiplicity = static_cast<int>(round(
              warmStart.edges[edgeIndex(day, vehicle, from, to, vehicles,
                                        nodes)]));
          if (multiplicity > 0) {
            solution.routingCost +=
                instance.Distance[from][to] * multiplicity;
            solution.routeEdges.push_back(
                {from, to, vehicle, day, multiplicity});
          }
        }

  if (solution.deliveredQuantity > 1e-9)
    solution.logisticRatio =
        solution.routingCost / solution.deliveredQuantity;
  return solution;
}
} // namespace

static string csvField(const string &value) {
  string escaped = "\"";
  for (char character : value) {
    if (character == '"')
      escaped += "\"\"";
    else
      escaped += character;
  }
  escaped += '"';
  return escaped;
}

static void printSolutionDetails(const input &instance,
                                 const exact::Solution &solution) {
  if (!solution.hasIncumbent) {
    cout << "  No feasible incumbent; detailed solution is unavailable.\n";
    return;
  }

  for (int day = 0; day < instance.TimeHorizon; ++day) {
    cout << "  Day " << day + 1 << ":\n";
    for (int vehicle = 0; vehicle < instance.NumberOfVehicles; ++vehicle) {
      vector<vector<size_t>> adjacency(instance.NumberOfRetailers + 1);
      vector<int> multiplicities;
      vector<pair<int, int>> endpoints;
      for (const auto &edge : solution.routeEdges)
        if (edge.day == day && edge.vehicle == vehicle) {
          const size_t edgeIndex = endpoints.size();
          endpoints.push_back({edge.from, edge.to});
          multiplicities.push_back(edge.multiplicity);
          adjacency[edge.from].push_back(edgeIndex);
          adjacency[edge.to].push_back(edgeIndex);
        }

      cout << "    Vehicle " << vehicle + 1 << " route: ";
      if (endpoints.empty()) {
        cout << "no route\n";
      } else {
        vector<int> used(endpoints.size(), 0);
        vector<int> route = {0};
        int current = 0;
        while (true) {
          size_t selected = endpoints.size();
          for (size_t edgeIndex : adjacency[current])
            if (used[edgeIndex] < multiplicities[edgeIndex]) {
              selected = edgeIndex;
              break;
            }
          if (selected == endpoints.size())
            break;

          ++used[selected];
          const auto &endpoint = endpoints[selected];
          current = endpoint.first == current ? endpoint.second : endpoint.first;
          route.push_back(current);
          if (current == 0)
            break;
        }
        for (size_t i = 0; i < route.size(); ++i) {
          if (i > 0)
            cout << " -> ";
          cout << (route[i] == 0 ? "Depot"
                                 : "Retailer " + to_string(route[i]));
        }
        cout << '\n';
      }

      bool hasDeliveries = false;
      for (const auto &delivery : solution.deliveries)
        if (delivery.day == day && delivery.vehicle == vehicle) {
          cout << "      Retailer " << delivery.customer + 1
               << " delivery: " << delivery.quantity << '\n';
          hasDeliveries = true;
        }
      if (!hasDeliveries)
        cout << "      No deliveries\n";
    }

    cout << "    End-of-day inventory - Depot: "
         << solution.inventoryLevels[0][day];
    for (int customer = 0; customer < instance.NumberOfRetailers; ++customer)
      cout << ", Retailer " << customer + 1 << ": "
           << solution.inventoryLevels[customer + 1][day];
    cout << '\n';
  }
}

int main() {
  file read_file;
  read_file.ReadDirectory();
  JSONDirectory = MachineDirectory + "Curtin/IRPLR/IRPLR/JSON/";
  read_file.ReadIRPInstanceName();
  read_file.ReadGlobalParameter();

  ofstream resultsFile("results.csv");
  if (!resultsFile.is_open()) {
    cerr << "Unable to open results.csv for writing" << endl;
    return 1;
  }
  resultsFile << "instance,number_of_retailers,number_of_vehicles,time_horizon,"
                 "warm_start_precheck_status,warm_start_error,"
                 "warm_start_routing_cost,warm_start_delivered_quantity,"
                 "warm_start_logistic_ratio,"
                 "cplex_status,cplex_status_code,cplex_has_incumbent,"
                 "cplex_error,cplex_routing_cost,"
                 "cplex_delivered_quantity,cplex_logistic_ratio,"
                 "routing_cost_difference,delivered_quantity_difference,"
                 "logistic_ratio_difference\n";
  resultsFile << setprecision(numeric_limits<double>::max_digits10);

  const double timeLimit = 3600.0;
  const double memoryLimit = 1024.0 * 1024.0 * 1024.0;
  for (size_t i = 0; i < read_file.instances.size(); ++i)
    for (size_t j = 0; j < read_file.instances[i].size(); ++j) {
      cout << "@ ---------------------------------------------" << endl;
      cout << "@ " << read_file.instances[i][j] << endl;
      cout << "@ ---------------------------------------------" << endl;

      input instance;
      instance.ReadIRPInstance(read_file.instances[i][j], read_file.InstanceType,
                               read_file.InstanceDirectories[i]);
      instance.PrintData();

      exact model;
      double instanceTimeLimit = timeLimit;
      double instanceMemoryLimit = memoryLimit;
      exact::WarmStart warmStart;
      exact::Solution warmStartSolution;
      string warmStartError;
      bool warmStartFeasible = false;
      try {
        warmStart =
            readWarmStart(instance, read_file.InstanceDirectories[i]);
        warmStartSolution = solutionFromWarmStart(instance, warmStart);
        warmStartFeasible = true;
      } catch (const exception &error) {
        warmStartError = error.what();
        cerr << "Cannot use the solution for instance " << instance.InstanceName
             << ": " << warmStartError << endl;
      }
      exact::Solution solution;
      if (warmStartFeasible)
        solution = model.IRPMODEL(instance, instanceTimeLimit,
                                  instanceMemoryLimit, &warmStart);
      const double routingCostDifference =
          solution.routingCost - warmStartSolution.routingCost;
      const double deliveredQuantityDifference =
          solution.deliveredQuantity - warmStartSolution.deliveredQuantity;
      const double logisticRatioDifference =
          solution.logisticRatio - warmStartSolution.logisticRatio;
      cout << "instance: " << instance.InstanceName
           << ", retailers: " << instance.NumberOfRetailers
           << ", vehicles: " << instance.NumberOfVehicles
           << ", time horizon: " << instance.TimeHorizon
           << ", JSON warm-start precheck: "
           << (warmStartFeasible ? "passed" : "failed")
           << ", exact CPLEX status: "
           << (warmStartFeasible ? solution.cplexStatus : "NOT_RUN");
      if (!solution.cplexError.empty())
        cout << " (" << solution.cplexError << ")";
      if (warmStartFeasible)
        cout << ", JSON routing cost: " << warmStartSolution.routingCost;
      if (solution.hasIncumbent)
        cout << ", CPLEX routing cost: "
             << solution.routingCost
             << ", CPLEX delivered quantity: " << solution.deliveredQuantity
             << ", CPLEX logistic ratio: " << solution.logisticRatio;
      if (warmStartFeasible && solution.hasIncumbent)
        cout << ", CPLEX minus JSON routing cost: "
             << routingCostDifference;
      cout << endl;
      printSolutionDetails(instance, solution);

      resultsFile << csvField(instance.InstanceName) << ','
                  << instance.NumberOfRetailers << ','
                  << instance.NumberOfVehicles << ','
                  << instance.TimeHorizon << ','
                  << (warmStartFeasible ? "VALIDATED" : "INVALID") << ','
                  << csvField(warmStartError) << ',';
      if (warmStartFeasible) {
        resultsFile << warmStartSolution.routingCost << ','
                    << warmStartSolution.deliveredQuantity << ','
                    << warmStartSolution.logisticRatio;
      } else {
        resultsFile << ",,";
      }
      resultsFile << ','
                  << (!warmStartFeasible
                          ? "NOT_RUN_INVALID_WARM_START"
                          : solution.cplexStatus)
                  << ',';
      if (warmStartFeasible && solution.cplexStatusCode >= 0)
        resultsFile << solution.cplexStatusCode;
      resultsFile << ','
                  << (warmStartFeasible
                          ? (solution.cplexHasIncumbent ? "YES" : "NO")
                          : "NOT_RUN")
                  << ','
                  << csvField(solution.cplexError) << ',';
      if (solution.hasIncumbent) {
        resultsFile << solution.routingCost << ','
                    << solution.deliveredQuantity << ','
                    << solution.logisticRatio;
      } else {
        resultsFile << ",,";
      }
      resultsFile << ',';
      if (warmStartFeasible && solution.hasIncumbent)
        resultsFile << routingCostDifference << ','
                    << deliveredQuantityDifference << ','
                    << logisticRatioDifference;
      else
        resultsFile << ",,";
      resultsFile << '\n';
    }

  resultsFile.close();
  if (!resultsFile) {
    cerr << "Failed while writing results.csv" << endl;
    return 1;
  }
  return 0;
}
