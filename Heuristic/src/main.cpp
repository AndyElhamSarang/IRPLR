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
unsigned int RandomSeed = 0;
base_generator_type generator(RandomSeed);
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
int DisturbStrength = 0;

// CSV columns written once per start (by INITIAL_ZONE, main and IteratedLocalSearch).
// Used for the header and to pad starts that are skipped when the time limit is reached.
const string PerStartColumns = "InitialCost,InitialQuantity,InitialLogisticRatio,T_InitialSchedule,InitialCostAfterNearestInsertion,InitialQuantityAfterNearestInsertion,InitialLogisticRatioAfterNearestInsertion,T_InitialScheduleAfterNearestInsertion,InitialCostAfterHGS,InitialQuantityAfterHGS,InitialLogisticRatioAfterHGS,InitialTimeForHGS,T_InitialSolution,NumberOfRebalance,NumberOfFeasibleRebalance,NumberOfRebalanceImproved,RebalanceAveragePercentageImprovement,RebalanceMaxPercentageImprovement,RebalanceMinPercentageImprovement,BestCost,BestQuantity,BestLogisticRatio,BestCostAfterHGS,BestQuantityAfterHGS,BestLogisticRatioAfterHGS,TimeForHGS,Time";
const int NumberOfPerStartColumns = count(PerStartColumns.begin(), PerStartColumns.end(), ',') + 1;

int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalSettings();
	read_file.ReadGlobalParameter();
	for (int experiment = 0; experiment < NumberOfExperiments; experiment++)
	{
		RandomSeed = static_cast<unsigned int>(time(0));
		generator.seed(RandomSeed);

		if (OutputResults == 1)
		{
			string experiment_str = to_string(experiment + 1);
			string experiment_name = "test";
			string file_type = ".csv";
			string file_name = experiment_name + "_exp" + experiment_str + file_type;
			Table.open(file_name);
			Table << "Seed," << RandomSeed << "\n";
			Table << "Global Parameters"
				  << ",NumberOfInitialSolutions," << NumberOfInitialSolutions
				  << ",MinGridResolution," << min_grid_resolution
				  << ",GridResolutionIncrement," << grid_resolution_increment
				  << ",NumberOfExperiments," << NumberOfExperiments
				  << ",InitialSolutionMethod," << InitialSolutionMethod
				  << ",AllowLagrangianRelaxation," << AllowLagrangianRelaxation
				  << ",TypeOfRebalance," << TypeOfRebalance
				  << ",InitialLagrangianScalar," << InitialLagrangianScalar
				  << ",ToAdjustLagrangianScalar," << ToAdjustLagrangianScalar
				  << ",ToTriggerAdjustment," << ToTriggerAdjustment
				  << ",ActivateHGS," << ActivateHGS
				  << ",Activate_Disturbance," << Activate_Disturbance
				  << ",theta," << theta
				  << ",sigma," << sigma
				  << ",phi," << phi
				  << ",DisturbStrength," << DisturbStrength
				  << ",MainAlgorithmTimeLimit," << MainAlgorithmTimeLimit << "\n";
			Table << ",#TimePeriods,#Nodes,#Vehicle";
			for (int i = 0; i < NumberOfInitialSolutions; i++)
			{
				Table << "," << PerStartColumns;
			}

			Table << ",FirstImpCost,FirstImpQuantity,FirstImpLogisticRatio,TimeAtFirstImprovement";
			for (int checkpoint = 30; checkpoint <= MainAlgorithmTimeLimit; checkpoint += 30)
			{
				Table << ",BestCostAt" << checkpoint << "s,BestQuantityAt" << checkpoint << "s,BestLogisticRatioAt" << checkpoint << "s,TimeAt" << checkpoint << "s";
			}
			Table << ",GlobalBestCost,GlobalBestQuantity,GlobalBestLogisticRatio,T_iteration,T_To_best,T_total_for_HGS,T_Total,NumberOfRestarts\n";
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
				Global_total_iteration = 0;
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
				// Stop launching new starts once the time budget is used up (same rule as the
				// irace tuning binary). NumberOfInitialSolutions is an upper limit on starts.
				int NumberOfStartsRun = 0;
				for (int j = 0; j < NumberOfInitialSolutions && difftime(time(NULL), total_start_time) < MainAlgorithmTimeLimit; j++)
				{
					NumberOfStartsRun++;
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
					Metaheuristic.IteratedLocalSearch(IRPLR, IRPSolution, Routing, memory, GlobalBest, FirstImprovementSolution, IRPSolution30s, IRPSolution60s, TimeCheckpointSolutions, read_file, j);
				}
				// Starts skipped because the time limit was reached: fill their columns with 0
				// so the row keeps the same layout as the header.
				if (OutputResults == 1)
				{
					for (int j = NumberOfStartsRun; j < NumberOfInitialSolutions; j++)
					{
						for (int c = 0; c < NumberOfPerStartColumns; c++)
						{
							Table << 0 << ",";
						}
					}
				}
				cout << "Number of starts run: " << NumberOfStartsRun << " of " << NumberOfInitialSolutions << endl;
				time(&total_end_time);
				double accum_time = difftime(total_end_time, total_start_time);
				cout << "Global best" << endl;
				GlobalBest.print_solution(IRPLR);
				GlobalBest.Validation(IRPLR);
				cout << "whether_results_reported at 30s: " << whether_results_reported_30 << ", whether_results_reported at 60s: " << whether_results_reported_60 << ", whether_results_reported at first improvement: " << whether_results_reported_first_improvement << endl;

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

					Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << Global_total_iteration << "," << GlobalBest.solution_time << "," << AccumulatedTimeHGS << "," << accum_time << "," << NumberOfStartsRun << ",";
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