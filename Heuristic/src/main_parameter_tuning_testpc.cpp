#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule =0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;
int Global_total_iteration = 0;
int NumberOfBalacingOperatorCalled = 0;
int NumberOfVehicleAtMinimumDelivery = 0;

double MainAlgorithmTimeLimit = 65.0; // Default local search time limit is 12 seconds for each initial solution
double AccumulatedTimeHGS = 0.0;
double power = 2.0;
ofstream Table;
base_generator_type generator(static_cast<unsigned int>(time(0)));
string MachineDirectory;
int OutputResults;
time_t start_time;
time_t end_time;
time_t start_time_to_best;
time_t end_time_to_best;
time_t total_start_time;
time_t total_end_time;
time_t HGS_start_time;
time_t HGS_end_time;
bool whether_results_reported_30 = false;
bool whether_results_reported_60 = false;
bool whether_results_reported_first_improvement = false;
string OutputSolutionJSON;
//Parameters
int NumberOfInitialSolutions=0;
int NumberOfExperiments =0;
string AllowLagrangianRelaxation;
string TypeOfRebalance;
double InitialLagrangianScalar = 0;
double ToAdjustLagrangianScalar = 0;
int ToTriggerAdjustment = 0;
int GridResolution = 0;
string ActivateHGS;
int theta=0;
int sigma=0; 
int phi=0;
int main(int argc, char* argv[])
{
	// Support both original positional arguments and modern `--flag value` style.
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " <instance> <NumberOfInitialSolutions> ...\n       or: " << argv[0] << " --instance <file> --NumberOfInitialSolutions <n> ...\n";
		return 1;
	}

	string instance;
	// If the first positional argument is not a flag, treat it as the instance path.
	if (argc > 1 && strncmp(argv[1], "--", 2) != 0)
	{
		instance = argv[1];
	}

	// Detect whether any long-style flags are present among the arguments
	bool any_flag = false;
	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "--", 2) == 0)
		{
			any_flag = true;
			break;
		}
	}

	if (any_flag)
	{
		for (int i = 1; i < argc; i++)
		{
			string opt = argv[i];
			if (opt.rfind("--", 0) == 0)
				opt = opt.substr(2);
			else
				continue;

			if (i + 1 >= argc)
			{
				cerr << "Missing value for option --" << opt << endl;
				return 1;
			}
			string val = argv[++i];

			if (opt == "instance") instance = val;
			else if (opt == "NumberOfInitialSolutions" || opt == "NumberOfInitialSolution") { NumberOfInitialSolutions = atoi(val.c_str()); }
			else if (opt == "NumberOfExperiments") { NumberOfExperiments = atoi(val.c_str()); }
			else if (opt == "AllowLagrangianRelaxation") { AllowLagrangianRelaxation = val; }
			else if (opt == "TypeOfRebalance") { TypeOfRebalance = val; }
			else if (opt == "InitialLagrangianScalar") { InitialLagrangianScalar = atof(val.c_str()); }
			else if (opt == "ToAdjustLagrangianScalar") { ToAdjustLagrangianScalar = atof(val.c_str()); }
			else if (opt == "ToTriggerAdjustment") { ToTriggerAdjustment = atoi(val.c_str()); }
			else if (opt == "GridResolution" || opt == "Grid_resolution_in_initial_solution_construction") { GridResolution = atoi(val.c_str()); }
			else if (opt == "ActivateHGS" || opt == "Activate HGS") { ActivateHGS = val; }
			else if (opt == "theta") { theta = atoi(val.c_str()); }
			else if (opt == "sigma") { sigma = atoi(val.c_str()); }
			else if (opt == "phi") { phi = atoi(val.c_str()); }
			else if (opt == "MainAlgorithmTimeLimit") { MainAlgorithmTimeLimit = atof(val.c_str()); }
			else if (opt == "OutputResults") { OutputResults = atoi(val.c_str()); }
			else if (opt == "OutputSolutionJSON") { OutputSolutionJSON = val; }
			else
			{
				cerr << "Warning: unknown option --" << opt << " (ignored)" << endl;
			}
		}

		cout << "Instance: " << instance << endl;
		cout << "Number of Initial Solutions: " << NumberOfInitialSolutions << endl;
		cout << "Number of Experiments: " << NumberOfExperiments << endl;
		cout << "Allow Lagrangian Relaxation: " << AllowLagrangianRelaxation << endl;
		cout << "Type of Rebalance: " << TypeOfRebalance << endl;
		cout << "Initial Lagrangian Scalar: " << InitialLagrangianScalar << endl;
		cout << "To Adjust Lagrangian Scalar: " << ToAdjustLagrangianScalar << endl;
		cout << "To Trigger Adjustment: " << ToTriggerAdjustment << endl;
		cout << "Grid Resolution: " << GridResolution << endl;
		cout << "Activate HGS: " << ActivateHGS << endl;
		cout << "Theta: " << theta << endl;
		cout << "Sigma: " << sigma << endl;
		cout << "Phi: " << phi << endl;
		cout << "Main Algorithm Time Limit: " << MainAlgorithmTimeLimit << endl;
	}
	else
	{
		if (argc < 15)
		{
			cerr << "Usage: " << argv[0] << " <instance> <NumberOfInitialSolutions> <NumberOfExperiments> <AllowLagrangianRelaxation> <TypeOfRebalance> <InitialLagrangianScalar> <ToAdjustLagrangianScalar> <ToTriggerAdjustment> <GridResolution> <ActivateHGS> <theta> <sigma> <phi> <MainAlgorithmTimeLimit>\n";
			return 1;
		}

		instance = argv[1];
		NumberOfInitialSolutions = atoi(argv[2]);
		cout << "Number of Initial Solutions: " << NumberOfInitialSolutions << endl;
		NumberOfExperiments = atoi(argv[3]);
		cout << "Number of Experiments: " << NumberOfExperiments << endl;
		AllowLagrangianRelaxation= argv[4];
		cout << "Allow Lagrangian Relaxation: " << AllowLagrangianRelaxation << endl;
		TypeOfRebalance= argv[5];
		cout << "Type of Rebalance: " << TypeOfRebalance << endl;
		InitialLagrangianScalar = atof(argv[6]);
		cout << "Initial Lagrangian Scalar: " << InitialLagrangianScalar << endl;
		ToAdjustLagrangianScalar = atof(argv[7]);
		cout << "To Adjust Lagrangian Scalar: " << ToAdjustLagrangianScalar << endl;
		ToTriggerAdjustment = atoi(argv[8]);
		cout << "To Trigger Adjustment: " << ToTriggerAdjustment << endl;
		GridResolution = atoi(argv[9]);
		cout << "Grid Resolution: " << GridResolution << endl;
		ActivateHGS = argv[10];
		cout << "Activate HGS: " << ActivateHGS << endl;
		theta = atoi(argv[11]);
		cout << "Theta: " << theta << endl;
		sigma = atoi(argv[12]);
		cout << "Sigma: " << sigma << endl;
		phi = atoi(argv[13]);
		cout << "Phi: " << phi << endl;
		MainAlgorithmTimeLimit = atof(argv[14]);
		cout << "Main Algorithm Time Limit: " << MainAlgorithmTimeLimit << endl;
	}

	file read_file;

	MachineDirectory = "/home/andytestpc/Desktop/";
    // JSONDirectory = MachineDirectory + "Curtin/IRPLR/IRPLR/JSON/";
	read_file.InstanceType = "Multiple vehicles IRP";

	vector<string> temp_instances;
	temp_instances.push_back(instance);
    read_file.instances.push_back(temp_instances);
	// Extract instance directory from full path so ReadIRPInstance receives the expected relative directory
	string instance_dir = "";
	size_t slash_pos = instance.find_last_of('/');
	if (slash_pos != string::npos)
	{
		// If the provided instance path starts with MachineDirectory, store the relative directory
		if (instance.rfind(MachineDirectory, 0) == 0)
		{
			instance_dir = instance.substr(MachineDirectory.size(), slash_pos - MachineDirectory.size() + 1);
		}
		else
		{
			// Fallback: remove leading slash to form a path similar to InstanceDirectories entries
			instance_dir = instance.substr(0, slash_pos + 1);
			if (!instance_dir.empty() && instance_dir[0] == '/')
				instance_dir = instance_dir.substr(1);
		}
	}
	else
	{
		instance_dir = "";
	}
	read_file.InstanceDirectories.push_back(instance_dir);

	read_file.ReadGlobalSettings();
	for (int experiment = 0; experiment < NumberOfExperiments; experiment++)
	{
		if (OutputResults == 1)
		{
			string experiment_str = to_string(experiment+1);
			string experiment_name = "MS";
			string file_type = ".csv";
			string file_name = experiment_name + "_exp" + experiment_str + file_type;
			Table.open(file_name);
			Table << ",#TimePeriods,#Nodes,#Vehicle";
			for (int i = 0; i < NumberOfInitialSolutions; i++)
			{
				Table << ",InitialCost,InitialQuantity,InitialLogisticRatio,T_InitialSchedule,InitialCostAfterHGS,InitialQuantityAfterHGS,InitialLogisticRatioAfterHGS,InitialTimeForHGS,T_InitialSolution,NumberOfRebalance,NumberOfFeasibleRebalance,NumberOfRebalanceImproved,RebalanceAveragePercentageImprovement,RebalanceMaxPercentageImprovement,RebalanceMinPercentageImprovement,BestCost,BestQuantity,BestLogisticRatio,BestCostAfterHGS,BestQuantityAfterHGS,BestLogisticRatioAfterHGS,TimeForHGS,Time";
			}

			Table << ",FirstImpCost,FirstImpQuantity,FirstImpLogisticRatio,TimeAtFirstImprovement,BestCostAt30s,BestQuantityAt30s,BestLogisticRatioAt30s,TimeAt30s,BestCostAt60s,BestQuantityAt60s,BestLogisticRatioAt60s,TimeAt60s,GlobalBestCost,GlobalBestQuantity,GlobalBestLogisticRatio,T_iteration,T_To_best,T_total_for_HGS,T_Total\n";
		}

		for (int i = 0; i < read_file.instances.size(); i++)
		{
			for (int j = 0; j < read_file.instances[i].size(); j++)
			{
			
				cout << "@ ---------------------------------------------" << endl;
				cout << "@ " << read_file.instances[i][j] << endl;
				cout << "@ ---------------------------------------------" << endl;

				input IRPLR;
				IRPLR.ReadIRPInstance(read_file.instances[i][j], read_file.InstanceType, read_file.InstanceDirectories[i]);
				if (printout_inputdata == 1)
				{
					IRPLR.PrintData();
				}

				HGS Routing;
				Routing.ReadParameter();
				Routing.HGSTimelimit = Routing.HGSTimelimit * (IRPLR.NumberOfRetailers - 1); // Set the time limit to be the number of retailers times the original time limit
				// MainAlgorithmTimeLimit = MainAlgorithmTimeLimit * NumberOfInitialSolutions;
				cout << "HGS timelimit: " << Routing.HGSTimelimit << endl;
				cout << "Local search timelimit: " << MainAlgorithmTimeLimit << endl;
				AccumulatedTimeHGS = 0.0; // Reset accumulated time for HGS for each instance
				if (OutputResults == 1)
				{
					Table << IRPLR.InstanceName  << "," << IRPLR.TimeHorizon << "," << IRPLR.NumberOfRetailers << "," << IRPLR.NumberOfVehicles << ","; // Print instance feastures in the table
				}
				////////////////////////////////////////////////////////////////
				//                                                            //
				//		     Preprocessing of global data structure           //
				//                                                            //
				////////////////////////////////////////////////////////////////
				preprocessing memory;
				memory.PopulateGlobalDataStructure(IRPLR);
				memory.CustomerInfo(IRPLR);
				// memory.PrintGlobalDataStructure();

				////////////////////////////////////////////////////////////////
				//                                                            //
				//		     Generate multiple initial solutions              //
				//		     		and start Multi-start                     //
				//                                                            //
				////////////////////////////////////////////////////////////////
				solution GlobalBest;
				GlobalBest.LogisticRatio = numeric_limits<double>::max(); // Set a large value
				solution FirstImprovementSolution;
				solution IRPSolution30s;
				solution IRPSolution60s;

				whether_results_reported_30 = false;
				whether_results_reported_60 = false;
				whether_results_reported_first_improvement = false;

				time(&total_start_time);
				time(&start_time_to_best);
				for (int j = 0; j < NumberOfInitialSolutions; j++)
				{
					cout << "Attempt:" << j << endl;

					solution_construction initial_solution;
					solution IRPSolution;

					time(&start_time);
					// initial_solution.INITIAL(IRPLR, IRPSolution, Routing);

					generator.seed(12345 + j * 789); // Different seed for each initial solution
					initial_solution.INITIAL_ZONE(IRPLR, IRPSolution, Routing, GlobalBest, read_file,j);
					time(&end_time);
					double total_time = difftime(end_time, start_time);
					if (OutputResults == 1)
					{
						// Record time consumed for initial solution construction
						Table << total_time << ",";
					}
					cout << "!Initial solution " << j + 1 << endl;
					IRPSolution.print_solution(IRPLR);
					
					IRPSolution.GetLogisticRatio(IRPLR);
					cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << endl;
					IRPSolution.Validation(IRPLR);
					generator.seed(static_cast<unsigned int>(time(0)));
					solution_improvement Metaheuristic;
					// Metaheuristic.LargeNeighbourhoodSearch(IRPLR, IRPSolution, Routing, memory); //Previously tested code.
					Metaheuristic.IteratedLocalSearch(IRPLR, IRPSolution, Routing, memory, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s,read_file, j);

					
				}
				time(&total_end_time);
				double accum_time = difftime(total_end_time, total_start_time);
				GlobalBest.Validation(IRPLR);
				cout << "whether_results_reported at 30s: " << whether_results_reported_30 << ", whether_results_reported at 60s: " << whether_results_reported_60 << ", whether_results_reported at first improvement: " << whether_results_reported_first_improvement << endl;

				cout << "Global best" << endl;				
				GlobalBest.print_solution(IRPLR);
				cout << "BestTransportationCost:" << GlobalBest.TotalTransportationCost << "\t TotalDelivery:" << GlobalBest.TotalDelivery << "\t LogistcRatio:" << GlobalBest.LogisticRatio << endl;
				cout << "IRACE_COST "<<GlobalBest.LogisticRatio << endl;
				if(OutputSolutionJSON == "YES")
				{
					GlobalBest.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName+ "_global_best.json");
				}
				if (whether_results_reported_first_improvement == false)
				{
					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << GlobalBest.solution_time << ",";
				}
				else
				{
					Table << FirstImprovementSolution.TotalTransportationCost << "," << FirstImprovementSolution.TotalDelivery << "," << FirstImprovementSolution.LogisticRatio << "," << FirstImprovementSolution.solution_time << ",";
				}
				if (whether_results_reported_30 == false)
				{
					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << GlobalBest.solution_time << ",";
				}
				else
				{
					Table << IRPSolution30s.TotalTransportationCost << "," << IRPSolution30s.TotalDelivery << "," << IRPSolution30s.LogisticRatio << "," << IRPSolution30s.solution_time << ",";
				}
				if (whether_results_reported_60 == false)
				{
					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << GlobalBest.solution_time << ",";
				}
				else
				{
					Table << IRPSolution60s.TotalTransportationCost << "," << IRPSolution60s.TotalDelivery << "," << IRPSolution60s.LogisticRatio << "," << IRPSolution60s.solution_time << ",";
				}
				if (OutputResults == 1)
				{

					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << Global_total_iteration << "," << GlobalBest.solution_time << "," << AccumulatedTimeHGS << "," << accum_time << ",";
				}
				if (OutputResults == 1)
				{
					Table << "\n";
				}
			}
		}
		Table.close();
	}

	return 0;
}