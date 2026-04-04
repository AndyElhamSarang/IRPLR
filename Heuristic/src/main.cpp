#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule = 0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;

int NumberOfBalacingOperatorCalled = 0;
int NumberOfVehicleAtMinimumDelivery = 0;

double LocalSearchTimeLimit = 36000.0; // Default local search time limit is 12 seconds for each initial solution

double power = 2.0;
ofstream Table;
base_generator_type generator(static_cast<unsigned int>(time(0)));
string MachineDirectory;
int OutputResults;
int NumberOfInitialSolutions;
time_t start_time;
time_t end_time;
time_t start_time_to_best;
time_t end_time_to_best;
time_t LS_start_time;
time_t LS_end_time;
time_t total_start_time;
time_t total_end_time;
bool whether_results_reported_30 = false;
bool whether_results_reported_60 = false;
int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalParameter();

	if (OutputResults == 1)
	{
		Table.open("MS.csv");
		Table << ",#TimePeriods,#Customer,#Vehicle";
		for(int i = 0; i<NumberOfInitialSolutions; i++)
		{
			Table<<",Cost,Quantity,LogisticRatio,T_InitialSchedule,CostAfterHGS,Quantity,LogisticRatio,T_InitialSolution,NumberOfRebalance,NumberOfRebalanceImproved,RebalanceAveragePercentageImprovement,BestCost,BestQuantity,BestLogisticRatio,Time";
		}

		Table << ",BestCostAt30s,BestQuantityAt30s,BestLogisticRatioAt30s,TimeAt30s,BestCostAt60s,BestQuantityAt60s,BestLogisticRatioAt60s,TimeAt60s,GlobalBestCost,GlobalBestQuantity,GlobalBestLogisticRatio,T_To_best,T_Total\n";
	}

	for (int i = 0; i < read_file.instances.size(); i++)
	{
		for (int j = 0; j < read_file.instances[i].size(); j++)
		{

			cout << "---------------------------------------------" << endl;
			cout << read_file.instances[i][j] << endl;
			cout << "---------------------------------------------" << endl;

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
			// LocalSearchTimeLimit = LocalSearchTimeLimit * NumberOfInitialSolutions;
			cout << "HGS timelimit: " << Routing.HGSTimelimit << endl;
			cout << "Local search timelimit: " << LocalSearchTimeLimit << endl;
			if (OutputResults == 1)
			{
				Table << read_file.instances[i][j] << "," << IRPLR.TimeHorizon << "," << IRPLR.NumberOfRetailers << "," << IRPLR.NumberOfVehicles << ","; // Print instance feastures in the table
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
			solution IRPSolution30s;
			solution IRPSolution60s;

			time(&total_start_time);
			time(&start_time_to_best);
			for (int j = 0; j < NumberOfInitialSolutions; j++)
			{
				cout << "Attempt:" << j << endl;

				solution_construction initial_solution;
				solution IRPSolution;

				time(&start_time);
				// initial_solution.INITIAL(IRPLR, IRPSolution, Routing);

				generator.seed(12345+j*789); // Different seed for each initial solution
				initial_solution.INITIAL_ZONE(IRPLR, IRPSolution, Routing, GlobalBest);
				time(&end_time);
				double total_time = difftime(end_time, start_time);
				if (OutputResults == 1)
				{
					//Record time consumed for initial solution construction
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
				Metaheuristic.IteratedLocalSearch(IRPLR, IRPSolution, Routing, memory, GlobalBest, IRPSolution30s, IRPSolution60s);

				
			}
			time(&total_end_time);
			double accum_time = difftime(total_end_time, total_start_time);
			cout<<"Global best"<<endl;
			GlobalBest.Validation(IRPLR);
			if(whether_results_reported_30 == false )
			{
				Table << "-,-,-,-,";
			
			}
			else
			{
				Table << IRPSolution30s.TotalTransportationCost << "," << IRPSolution30s.TotalDelivery << "," << IRPSolution30s.LogisticRatio << "," << IRPSolution30s.solution_time << ",";
			}
			if(whether_results_reported_60 == false)
			{
				Table << "-,-,-,-,";
			}
			else
			{
				Table << IRPSolution60s.TotalTransportationCost << "," << IRPSolution60s.TotalDelivery << "," << IRPSolution60s.LogisticRatio << "," << IRPSolution60s.solution_time << ",";
			}
			if (OutputResults == 1)
			{

				Table << GlobalBest.TotalTransportationCost << "," << GlobalBest.TotalDelivery << "," << GlobalBest.LogisticRatio << "," << GlobalBest.solution_time << "," << accum_time << ",";
			}
			if (OutputResults == 1)
			{
				Table << "\n";
			}
		}
	}

	return 0;
}