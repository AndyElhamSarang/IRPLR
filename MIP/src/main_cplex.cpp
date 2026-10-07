#include "lib.h"
#include "irplr_model.h"

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
                                 const IRPLRModel::Solution &solution) {
  if (solution.inventoryLevels.empty()) {
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
          if (selected == endpoints.size()) break;

          ++used[selected];
          const auto &endpoint = endpoints[selected];
          current = endpoint.first == current ? endpoint.second : endpoint.first;
          route.push_back(current);
          if (current == 0) break;
        }
        for (size_t i = 0; i < route.size(); ++i) {
          if (i > 0) cout << " -> ";
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
      if (!hasDeliveries) cout << "      No deliveries\n";
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
  read_file.ReadIRPInstanceName();
  read_file.ReadGlobalParameter();

  ofstream resultsFile("results.csv");
  if (!resultsFile.is_open()) {
    cerr << "Unable to open results.csv for writing" << endl;
    return 1;
  }
  resultsFile << "instance,number_of_retailers,number_of_vehicles,time_horizon,"
                 "routing_cost,delivered_quantity,logistic_ratio,optimality_gap\n";
  resultsFile << setprecision(numeric_limits<double>::max_digits10);

  IloEnv env;
  try {
    for (size_t i = 0; i < read_file.instances.size(); ++i)
      for (size_t j = 0; j < read_file.instances[i].size(); ++j) {
        input instance;
        instance.ReadIRPInstance(read_file.instances[i][j], read_file.InstanceType,
                                 read_file.InstanceDirectories[i]);
        instance.PrintData();
        IRPLRModel model(env, instance);
        IRPLRModel::Solution solution = model.minimizeRouting();
        cout << "instance: " << instance.InstanceName
             << ", retailers: " << instance.NumberOfRetailers
             << ", vehicles: " << instance.NumberOfVehicles
             << ", time horizon: " << instance.TimeHorizon
             << ", routing cost: " << solution.routingCost
             << ", delivered quantity: " << solution.deliveredQuantity
             << ", logistic ratio: " << solution.logisticRatio
             << ", optimality gap: " << solution.optimalityGap << endl;
        printSolutionDetails(instance, solution);
        resultsFile << csvField(instance.InstanceName) << ','
                    << instance.NumberOfRetailers << ','
                    << instance.NumberOfVehicles << ','
                    << instance.TimeHorizon << ','
                    << solution.routingCost << ','
                    << solution.deliveredQuantity << ','
                    << solution.logisticRatio << ','
                    << solution.optimalityGap << '\n';
      }
  } catch (IloException &exception) {
    cerr << "CPLEX error: " << exception << endl;
    env.end();
    return 1;
  } catch (...) {
    cerr << "Unknown error while solving the IRP-LR model" << endl;
    env.end();
    return 1;
  }
  env.end();
  resultsFile.close();
  if (!resultsFile) {
    cerr << "Failed while writing results.csv" << endl;
    return 1;
  }
  return 0;
}
