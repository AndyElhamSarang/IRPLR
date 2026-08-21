#include "lib.h"
int printout_inputdata = 0;
int printout_initialSchedule = 0;
int printout_initialOutputCVRP = 0;
int printout_initialRouting = 0;
int printout_initialReadCVRP = 0;
int printout_initial = 0;
double power = 2.0;
ofstream Table;
string MachineDirectory;
int OutputResults;
int main()
{
	file read_file;
	read_file.ReadDirectory();
	read_file.ReadIRPInstanceName();
	read_file.ReadGlobalParameter();

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

			exact exact_IRPLR;
			double time_limit = 3600;
			double memory_limit = 1024 * 1024 * 1024;
			exact_IRPLR.IRPMODEL(IRPLR, time_limit, memory_limit);
		}
	}

	return 0;
}