#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule = 0;
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
// Parameters
int NumberOfInitialSolutions = 0;
int NumberOfExperiments = 0;
string InitialSolutionMethod = "Initial_BlockZone_Schedule";
string AllowLagrangianRelaxation;
string TypeOfRebalance;
string Activate_Disturbance = "YES";
string LocalSearchOrder = "SEQUENTIAL";
bool UseVehicleSlack = true;
double InitialLagrangianScalar = 0;
double ToAdjustLagrangianScalar = 0;
int ToTriggerAdjustment = 0;
string ActivateHGS;
double theta = 0;
int sigma = 0;
int phi = 0;
int min_grid_resolution = 0;
int grid_resolution_increment = 0;
int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalSettings();
	read_file.ReadGlobalParameter();

	NumberOfInitialSolutions = 10;
	cout << "NumberOfInitialSolutions:" << NumberOfInitialSolutions << endl;

	NumberOfExperiments = 1;
	cout << "NumberOfExperiments:" << NumberOfExperiments << endl;

	min_grid_resolution = 4;
	cout << "min_grid_resolution:" << min_grid_resolution << endl;

	grid_resolution_increment = 5;
	cout << "grid_resolution_increment:" << grid_resolution_increment << endl;

	AllowLagrangianRelaxation = "YES";
	cout << "AllowLagrangianRelaxation:" << AllowLagrangianRelaxation << endl;

	TypeOfRebalance = "GUARANTEE_FEASIBILITY";
	cout << "TypeOfRebalance:" << TypeOfRebalance << endl;

	InitialLagrangianScalar = 10;
	cout << "InitialLagrangianScalar:" << InitialLagrangianScalar << endl;

	ToAdjustLagrangianScalar = 1;
	cout << "ToAdjustLagrangianScalar:" << ToAdjustLagrangianScalar << endl;

	ToTriggerAdjustment = 20;
	cout << "ToTriggerAdjustment:" << ToTriggerAdjustment << endl;

	ActivateHGS = "NO";
	cout << "ActivateHGS:" << ActivateHGS << endl;

	theta = 3;
	cout << "theta:" << theta << endl;

	sigma = 10;
	cout << "sigma:" << sigma << endl;

	phi = 10;
	cout << "phi:" << phi << endl;

	MainAlgorithmTimeLimit = 60;
	cout << "MainAlgorithmTimeLimit:" << MainAlgorithmTimeLimit << endl;
	// Inout parameter validation
	bool valid_input = true;
	if (OutputSolutionJSON != "YES" && OutputSolutionJSON != "NO")
	{
		cout << "Error: Invalid value for OutputSolutionJSON. Please set it to either 'YES' or 'NO'." << endl;
		valid_input = false;
	}
	if (NumberOfInitialSolutions <= 0)
	{
		cout << "Error: NumberOfInitialSolutions should be a positive integer." << endl;
		valid_input = false;
	}
	if (NumberOfExperiments <= 0)
	{
		cout << "Error: NumberOfExperiments should be a positive integer." << endl;
		valid_input = false;
	}
	if (AllowLagrangianRelaxation != "YES" && AllowLagrangianRelaxation != "NO")
	{
		cout << "Error: Invalid value for AllowLagrangianRelaxation. Please set it to either 'YES' or 'NO'." << endl;
		valid_input = false;
	}
	if (TypeOfRebalance != "GUARANTEE_FEASIBILITY" &&
		TypeOfRebalance != "NOT_GUARANTEE_FEASIBILITY" &&
		TypeOfRebalance != "NO_REBALANCE")
	{
		cout << "Error: Invalid value for TypeOfRebalance. Please set it to 'GUARANTEE_FEASIBILITY', 'NOT_GUARANTEE_FEASIBILITY', or 'NO_REBALANCE'." << endl;
		valid_input = false;
	}
	if (InitialLagrangianScalar <= 0)
	{
		cout << "Error: InitialLagrangianScalar should be positive." << endl;
		valid_input = false;
	}
	if (ToAdjustLagrangianScalar <= 0)
	{
		cout << "Error: ToAdjustLagrangianScalar should be positive." << endl;
		valid_input = false;
	}
	if (ToTriggerAdjustment <= 0)
	{
		cout << "Error: ToTriggerAdjustment should be a positive integer." << endl;
		valid_input = false;
	}
	if (ActivateHGS != "YES" && ActivateHGS != "NO")
	{
		cout << "Error: Invalid value for ActivateHGS. Please set it to either 'YES' or 'NO'." << endl;
		valid_input = false;
	}
	if (Activate_Disturbance != "YES" && Activate_Disturbance != "NO")
	{
		cout << "Error: Invalid value for Activate_Disturbance. Please set it to either 'YES' or 'NO'." << endl;
		valid_input = false;
	}
	if (valid_input == false)
	{
		exit(1);
	}
	for (int experiment = 0; experiment < NumberOfExperiments; experiment++)
	{
		if (OutputResults == 1)
		{
			string experiment_str = to_string(experiment + 1);
			string experiment_name = "test";
			string file_type = ".csv";
			string file_name = experiment_name + "_exp" + experiment_str + file_type;
			Table.open(file_name);
			Table << ",#TimePeriods,#Nodes,#Vehicle";
			for (int i = 0; i < NumberOfInitialSolutions; i++)
			{
				Table << ",InitialCost,InitialQuantity,InitialLogisticRatio,T_InitialSchedule,InitialCostAfterHGS,InitialQuantityAfterHGS,InitialLogisticRatioAfterHGS,InitialTimeForHGS,T_InitialSolution,NumberOfRebalance,NumberOfFeasibleRebalance,NumberOfRebalanceImproved,RebalanceAveragePercentageImprovement,RebalanceMaxPercentageImprovement,RebalanceMinPercentageImprovement,BestCost,BestQuantity,BestLogisticRatio,BestCostAfterHGS,BestQuantityAfterHGS,BestLogisticRatioAfterHGS,TimeForHGS,Time";
			}

			Table << ",FirstImpCost,FirstImpQuantity,FirstImpLogisticRatio,TimeAtFirstImprovement";
			for (int checkpoint = 30; checkpoint <= MainAlgorithmTimeLimit; checkpoint += 30)
			{
				Table << ",BestCostAt" << checkpoint << "s,BestQuantityAt" << checkpoint << "s,BestLogisticRatioAt" << checkpoint << "s,TimeAt" << checkpoint << "s";
			}
			Table << ",GlobalBestCost,GlobalBestQuantity,GlobalBestLogisticRatio,T_iteration,T_To_best,T_total_for_HGS,T_Total\n";
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
					Table << IRPLR.InstanceName << "," << IRPLR.TimeHorizon << "," << IRPLR.NumberOfRetailers << "," << IRPLR.NumberOfVehicles << ","; // Print instance feastures in the table
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
				vector<solution> TimeCheckpointSolutions;
				for (int checkpoint = 30; checkpoint <= MainAlgorithmTimeLimit; checkpoint += 30)
				{
					solution checkpointSolution;
					checkpointSolution.solution_time = -1;
					TimeCheckpointSolutions.push_back(checkpointSolution);
				}

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

					// generator.seed(12345 + j * 789); // Different seed for each initial solution
					initial_solution.INITIAL_ZONE(IRPLR, IRPSolution, Routing, GlobalBest, read_file, j);
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
					// generator.seed(static_cast<unsigned int>(time(0)));
					solution_improvement Metaheuristic;
					// Metaheuristic.LargeNeighbourhoodSearch(IRPLR, IRPSolution, Routing, memory); //Previously tested code.
					Metaheuristic.IteratedLocalSearch(IRPLR, IRPSolution, Routing, memory, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, read_file, j);
				}
				time(&total_end_time);
				double accum_time = difftime(total_end_time, total_start_time);
				GlobalBest.Validation(IRPLR);
				cout << "whether_results_reported at 30s: " << whether_results_reported_30 << ", whether_results_reported at 60s: " << whether_results_reported_60 << ", whether_results_reported at first improvement: " << whether_results_reported_first_improvement << endl;

				cout << "Global best" << endl;
				GlobalBest.print_solution(IRPLR);
				cout << "BestTransportationCost:" << GlobalBest.TotalTransportationCost << "\t TotalDelivery:" << GlobalBest.TotalDelivery << "\t LogistcRatio:" << GlobalBest.LogisticRatio << endl;

				if (OutputSolutionJSON == "YES")
				{
					GlobalBest.OutputJSON(IRPLR, read_file.JSONDirectory + IRPLR.InstanceName + "_global_best.json");
				}
				if (whether_results_reported_first_improvement == false)
				{
					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << GlobalBest.solution_time << ",";
				}
				else
				{
					Table << FirstImprovementSolution.TotalTransportationCost << "," << FirstImprovementSolution.TotalDelivery << "," << FirstImprovementSolution.LogisticRatio << "," << FirstImprovementSolution.solution_time << ",";
				}
				for (const solution &checkpointSolution : TimeCheckpointSolutions)
				{
					const solution &reportedSolution = checkpointSolution.solution_time < 0 ? GlobalBest : checkpointSolution;
					Table << reportedSolution.TotalTransportationCost << "," << reportedSolution.TotalDelivery << "," << reportedSolution.LogisticRatio << "," << reportedSolution.solution_time << ",";
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