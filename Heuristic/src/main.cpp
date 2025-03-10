#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule = 0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;

int NumberOfBalacingOperatorCalled=0;
int NumberOfVehicleAtMinimumDelivery =0;

double power = 2.0;
ofstream Table;
base_generator_type generator(static_cast<unsigned int>(time(0)));
string MachineDirectory;
int OutputResults;
int NumberOfInitialSolutions;
time_t start_time;
time_t end_time;
int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalParameter();

	HGS Routing;
	Routing.ReadParameter();

	if (OutputResults == 1)
	{
		Table.open("InitalBlockZone.csv");
		Table << ",#TimePeriods,#Customer,#Vehicle,Cost,Quantity,LogisticRatio,T_InitialSchedule,CostAfterHGS,Quantity,LogisticRatio,T_InitialSolution\n";
	}

	for (int i = 0; i < read_file.instances.size(); i++)
	{
		for (int j = 0; j < read_file.instances[i].size(); j++)
		{
			cout << "---------------------------------------------" << endl;
			cout << read_file.instances[i][j] << endl;
			cout << "---------------------------------------------" << endl;
			

			input IRPLR;
			IRPLR.ReadIRPInstance(read_file.instances[i][j], read_file.InstanceType, read_file.InstanceDirectories[i]);
			if (printout_inputdata == 1)
			{
				IRPLR.PrintData();
			}
			if (OutputResults == 1)
			{
				Table << read_file.instances[i][j] << ","<<IRPLR.TimeHorizon<<","<<IRPLR.NumberOfRetailers<<","<<IRPLR.NumberOfVehicles<<","; // Print instance feastures in the table
			}
			preprocessing memory;
			memory.PopulateGlobalDataStructure(IRPLR);

			memory.CustomerInfo(IRPLR);
			
			//memory.PrintGlobalDataStructure();
			for (int j = 0; j < NumberOfInitialSolutions; j++)
			{
				cout << "Attempt:" << j << endl;

				solution_construction initial_solution;
				solution IRPSolution;

				time(&start_time);
				//initial_solution.INITIAL(IRPLR, IRPSolution, Routing);
				initial_solution.INITIAL_ZONE(IRPLR, IRPSolution, Routing);				
				time(&end_time);
				IRPSolution.print_solution(IRPLR);

				solution_improvement Metaheuristic;
				Metaheuristic.LargeNeighbourhoodSearch(IRPLR, IRPSolution, Routing, memory);
				double total_time = difftime(end_time, start_time);
				if (OutputResults == 1)
				{
					Table << total_time << ",";
				}
			}
			if (OutputResults == 1)
			{
				Table << "\n";
			}
		}
	}

	return 0;
}