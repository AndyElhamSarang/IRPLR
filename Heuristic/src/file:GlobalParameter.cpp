#include "lib.h"
void file::ReadGlobalParameter()
{
    cout << "Read Global Parameter" << endl;
    string GlbalSetting = MachineDirectory + "Curtin/IRPLR/IRPLR/Heuristic/par/GlobalSetting.txt";
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

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeNumberOfExperiments(TakeParameter);
    ss_TakeNumberOfExperiments >> NumberOfExperiments;
    cout << dummyline << ":" << NumberOfExperiments << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeAllowLagrangianRelaxation(TakeParameter);
    ss_TakeAllowLagrangianRelaxation >> AllowLagrangianRelaxation;
    cout << dummyline << ":" << AllowLagrangianRelaxation << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeTypeOfRebalance(TakeParameter);
    ss_TakeTypeOfRebalance >> TypeOfRebalance;
    cout << dummyline << ":" << TypeOfRebalance << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeInitialLagrangianScalar(TakeParameter);
    ss_TakeInitialLagrangianScalar >> InitialLagrangianScalar;
    cout << dummyline << ":" << InitialLagrangianScalar << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeToAdjustLagrangianScalar(TakeParameter);
    ss_TakeToAdjustLagrangianScalar >> ToAdjustLagrangianScalar;
    cout << dummyline << ":" << ToAdjustLagrangianScalar << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeToTriggerAdjustment(TakeParameter);
    ss_TakeToTriggerAdjustment >> ToTriggerAdjustment;
    cout << dummyline << ":" << ToTriggerAdjustment << endl;
    //Inout parameter validation
    bool valid_input = true;
    if(NumberOfInitialSolutions <= 0)
    {
        cout << "Error: NumberOfInitialSolutions should be a positive integer." << endl;
        valid_input = false;
    }
    if(NumberOfExperiments <= 0)
    {
        cout << "Error: NumberOfExperiments should be a positive integer." << endl;
        valid_input = false;
    }
    if (AllowLagrangianRelaxation != "YES" && AllowLagrangianRelaxation != "NO")
    {
        cout << "Error: Invalid value for AllowLagrangianRelaxation. Please set it to either 'YES' or 'NO'." << endl;
        valid_input = false;
    }
    if (TypeOfRebalance != "GUARANTEE_FEASIBILITY" && TypeOfRebalance != "NOT_GUARANTEE_FEASIBILITY")
    {
        cout << "Error: Invalid value for TypeOfRebalance. Please set it to either 'GUARANTEE_FEASIBILITY' or 'NOT_GUARANTEE_FEASIBILITY'." << endl;
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
    if (valid_input == false)
    {
        exit(1);
    }
}