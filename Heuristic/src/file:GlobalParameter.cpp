#include "lib.h"
void file::ReadGlobalParameter()
{
    cout << "Read Global Parameter" << endl;
    string GlbalSetting = MachineDirectory + "Curtin/IRPLR/Heuristic/par/GlobalSetting.txt";
    ifstream ifGlobal;
    ifGlobal.open(GlbalSetting.c_str());

    string dummyline;
    string TakeParameter;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_inputdata(TakeParameter);
    ss_TakePrintout_inputdata >> printout_inputdata;
    cout << dummyline << ":";
    if (printout_inputdata == 0)
        cout << "No" << endl;
    else if (printout_inputdata == 1)
        cout << "Yes" << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_initialSchedule(TakeParameter);
    ss_TakePrintout_initialSchedule >> printout_initialSchedule;
    cout << dummyline << ":";
    if (printout_initialSchedule == 0)
        cout << "No" << endl;
    else if (printout_initialSchedule == 1)
        cout << "Yes" << endl;


    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_initialOutputCVRP(TakeParameter);
    ss_TakePrintout_initialOutputCVRP >> printout_initialOutputCVRP;
    cout << dummyline << ":";
    if (printout_initialOutputCVRP == 0)
        cout << "No" << endl;
    else if (printout_initialOutputCVRP == 1)
        cout << "Yes" << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_initialRouting(TakeParameter);
    ss_TakePrintout_initialRouting >> printout_initialRouting;
    cout << dummyline << ":";
    if (printout_initialRouting == 0)
        cout << "No" << endl;
    else if (printout_initialRouting == 1)
        cout << "Yes" << endl;

        getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_initialReadCVRP(TakeParameter);
    ss_TakePrintout_initialReadCVRP >> printout_initialReadCVRP;
    cout << dummyline << ":";
    if (printout_initialReadCVRP == 0)
        cout << "No" << endl;
    else if (printout_initialReadCVRP == 1)
        cout << "Yes" << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakePrintout_initial(TakeParameter);
    ss_TakePrintout_initial >> printout_initial;
    cout << dummyline << ":";
    if (printout_initial == 0)
        cout << "No" << endl;
    else if (printout_initial == 1)
        cout << "Yes" << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeOutputResults(TakeParameter);
    ss_TakeOutputResults >> OutputResults;
    cout << dummyline << ": ";
    if (OutputResults == 0)
        cout << "No" << endl;
    else if (OutputResults == 1)
        cout << "Yes" << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeNumberOfInitialSolutions(TakeParameter);
    ss_TakeNumberOfInitialSolutions >> NumberOfInitialSolutions;
    cout << dummyline << ":" << NumberOfInitialSolutions << endl;
}