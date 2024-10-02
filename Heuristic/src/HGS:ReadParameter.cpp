#include "lib.h"
void HGS::ReadParameter()
{

    cout << "Read HGS Setting" << endl;
    string HGS_Setting = MachineDirectory + "Curtin/IRPLR/Heuristic/par/HGS_Setting.txt";
    ifstream ifHGS;
    ifHGS.open(HGS_Setting.c_str());

    string dummyline;
    string TakeParameter;
    getline(ifHGS, dummyline);
    getline(ifHGS, TakeParameter);
    stringstream ss_TakeHGSMaxNonimproveIt(TakeParameter);
    ss_TakeHGSMaxNonimproveIt >> HGSMaxNonimproveIt;
    cout << dummyline << endl;
    if (HGSMaxNonimproveIt >= 0)
        cout << HGSMaxNonimproveIt << endl;
    else if (HGSMaxNonimproveIt < 0)
        cout << "Set as default" << endl;

    getline(ifHGS, dummyline);
    getline(ifHGS, TakeParameter);
    stringstream ss_TakeHGSTimelimit(TakeParameter);
    ss_TakeHGSTimelimit >> HGSTimelimit;
    cout << dummyline << endl;
    if (HGSTimelimit >= 0)
        cout << HGSTimelimit << endl;
    else if (HGSTimelimit < 0)
        cout << "Set as default" << endl;

    getline(ifHGS, dummyline);
    getline(ifHGS, TakeParameter);
    stringstream ss_TakeHGS_Seed(TakeParameter);
    ss_TakeHGS_Seed >> HGS_Seed;
    cout << dummyline << endl;
    if (HGS_Seed >= 0)
        cout << HGS_Seed << endl;
    else if (HGS_Seed < 0)
        cout << "Set as default" << endl;

    getline(ifHGS, dummyline);
    getline(ifHGS, TakeParameter);
    stringstream ss_TakeDistanceRounding(TakeParameter);
    ss_TakeDistanceRounding >> DistanceRounding;
    cout << dummyline << endl;
    if (DistanceRounding == 0)
        cout << "No" << endl;
    else if (DistanceRounding == 1)
        cout << "Yes" << endl;
    else if (DistanceRounding < 0)
        cout << "Set as default" << endl;

    getline(ifHGS, dummyline);
    getline(ifHGS, TakeParameter);
    stringstream ss_TakeHGS_Printout(TakeParameter);
    ss_TakeHGS_Printout >> HGS_Printout;
    cout << dummyline << endl;
    if (HGS_Printout == 0)
        cout << "No" << endl;
    else if (HGS_Printout == 1)
        cout << "Yes" << endl;
    else if (HGS_Printout < 0)
        cout << "Set as default" << endl;
}