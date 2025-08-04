#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule = 0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;

int NumberOfBalacingOperatorCalled = 0;
int NumberOfVehicleAtMinimumDelivery = 0;

double LocalSearchTimeLimit = 60.0; // Default local search time limit is 60 seconds

double power = 2.0;
ofstream Table;
base_generator_type generator(static_cast<unsigned int>(time(0)));
string MachineDirectory;
int OutputResults;
int NumberOfInitialSolutions;
time_t start_time;
time_t end_time;

time_t LS_start_time;
time_t LS_end_time;
int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalParameter();

	if (OutputResults == 1)
	{
		Table.open("MS_all_instances_HGSTL0.5.csv");
		Table << ",#TimePeriods,#Customer,#Vehicle,Cost,Quantity,LogisticRatio,T_InitialSchedule,CostAfterHGS,Quantity,LogisticRatio,T_InitialSolution,NumberOfRebalance,NumberOfRebalanceImproved,RebalanceAveragePercentageImprovement,BestCost,BestQuantity,BestLogisticRatio,T_Total\n";
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
			//LocalSearchTimeLimit = LocalSearchTimeLimit * (IRPLR.NumberOfRetailers - 1); 
			cout<<"HGS timelimit: "<<Routing.HGSTimelimit<<endl;
			if (OutputResults == 1)
			{
				Table << read_file.instances[i][j] << "," << IRPLR.TimeHorizon << "," << IRPLR.NumberOfRetailers << "," << IRPLR.NumberOfVehicles << ","; // Print instance feastures in the table
			}
			preprocessing memory;
			memory.PopulateGlobalDataStructure(IRPLR);

			memory.CustomerInfo(IRPLR);

			// memory.PrintGlobalDataStructure();
			for (int j = 0; j < NumberOfInitialSolutions; j++)
			{
				cout << "Attempt:" << j << endl;

				solution_construction initial_solution;
				solution IRPSolution;

				time(&start_time);
				// initial_solution.INITIAL(IRPLR, IRPSolution, Routing);
				initial_solution.INITIAL_ZONE(IRPLR, IRPSolution, Routing);
				time(&end_time);
				double total_time = difftime(end_time, start_time);
				if (OutputResults == 1)
				{
					Table << total_time << ",";
				}
				cout << "!Initial solution " << j + 1 << endl;
				IRPSolution.print_solution(IRPLR);

				base_generator_type generator(static_cast<unsigned int>(time(0)));
				solution_improvement Metaheuristic;
				Metaheuristic.LargeNeighbourhoodSearch(IRPLR, IRPSolution, Routing, memory);
			}
			if (OutputResults == 1)
			{
				Table << "\n";
			}
		}
	}

	return 0;
}