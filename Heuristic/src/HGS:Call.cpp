#include "lib.h"
void HGS::CallHGS(input &IRPLR)
{
    if (printout_initialRouting == 1)
    {
        cout << "Solve routing via HGS" << endl;
    }
    string DirectoryForHGS = MachineDirectory + "Curtin/IRPLR/HGS-CVRP-main/build/hgs";
    string InputCVRP = MachineDirectory + "Curtin/IRPLR/HGS-CVRP-main/Instances/IRP/IRP.vrp";
    string OutputCVRP = MachineDirectory + "Curtin/IRPLR/HGS-CVRP-main/Instances/IRP/IRPSolution.sol";
    string HGS_Parameter;
    if (HGSMaxNonimproveIt >= 0)
        HGS_Parameter += " -it " + to_string(HGSMaxNonimproveIt);
    if (HGSTimelimit >= 0)
        HGS_Parameter += +" -t " + to_string(HGSTimelimit);
    if (HGS_Seed >= 0)
        HGS_Parameter += +" -seed " + to_string(HGS_Seed);
    if (HGS_Printout >= 0)
        HGS_Parameter += +" -log " + to_string(HGS_Printout);
    if (DistanceRounding >= 0)
        HGS_Parameter += +" -round " + to_string(DistanceRounding);

    string SetNumberOfVehicle = " -veh " + to_string(IRPLR.NumberOfVehicles);
    DirectoryForHGS = DirectoryForHGS + " " + InputCVRP + " " + OutputCVRP + " " + SetNumberOfVehicle + " " + HGS_Parameter;
    if (printout_initialRouting == 1)
    {
        cout << "Call HGS with:" << endl;
        cout << DirectoryForHGS << endl;
    }
    int callingHGS = system(DirectoryForHGS.c_str());
}