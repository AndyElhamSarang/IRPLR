// #include "lib.h"
// #include "irplr_model.h"

// int printout_inputdata = 0;
// int printout_initialSchedule = 0;
// int printout_initialOutputCVRP = 0;
// int printout_initialRouting = 0;
// int printout_initialReadCVRP = 0;
// int printout_initial = 0;
// double power = 2.0;
// ofstream Table;
// string MachineDirectory;
// string JSONDirectory;
// int OutputResults = 0;

// int main() {
//   file read_file;
//   read_file.ReadDirectory();
//   read_file.ReadIRPInstanceName();
//   read_file.ReadGlobalParameter();

//   IloEnv env;
//   try {
//     for (size_t i = 0; i < read_file.instances.size(); ++i)
//       for (size_t j = 0; j < read_file.instances[i].size(); ++j) {
//         input instance;
//         instance.ReadIRPInstance(read_file.instances[i][j], read_file.InstanceType,
//                                  read_file.InstanceDirectories[i]);
//         IRPLRModel model(env, instance);
//         IRPLRModel::Solution solution = model.minimizeRouting();
//         cout << "routing cost: " << solution.routingCost
//              << ", delivered: " << solution.deliveredQuantity
//              << ", logistic ratio: " << solution.logisticRatio << endl;
//       }
//   } catch (IloException &exception) {
//     cerr << "CPLEX error: " << exception << endl;
//     env.end();
//     return 1;
//   } catch (...) {
//     cerr << "Unknown error while solving the IRP-LR model" << endl;
//     env.end();
//     return 1;
//   }
//   env.end();
//   return 0;
// }
