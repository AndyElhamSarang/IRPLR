#include "lib.h"
void file::ReadGlobalSettings()
{
    cout << "Read Global Settings" << endl;
    string GlobalSetting = MachineDirectory + "Curtin/IRPLR/IRPLR/Heuristic/par/GlobalSetting.txt";
    ifstream ifGlobal;
    ifGlobal.open(GlobalSetting.c_str());

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
    stringstream ss_TakeOutputSolutionJSON(TakeParameter);
    ss_TakeOutputSolutionJSON >> OutputSolutionJSON;
    cout << dummyline << ": " << OutputSolutionJSON << endl;

    

    //Inout parameter validation
    bool valid_input = true;
    if(OutputSolutionJSON != "YES" && OutputSolutionJSON != "NO")
    {
        cout << "Error: Invalid value for OutputSolutionJSON. Please set it to either 'YES' or 'NO'." << endl;
        valid_input = false;
    }
    
    if (valid_input == false)
    {
        exit(1);
    }
}


void file::ReadGlobalParameter()
{
    
   cout << "Read Global Parameter" << endl;
    string GlobalParameter = MachineDirectory + "Curtin/IRPLR/IRPLR/Heuristic/par/GlobalParameter.txt";
    ifstream ifGlobal;
    ifGlobal.open(GlobalParameter.c_str());

    string dummyline;
    string TakeParameter;
    
    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeNumberOfInitialSolutions(TakeParameter);
    ss_TakeNumberOfInitialSolutions >> NumberOfInitialSolutions;
    cout << dummyline << ":" << NumberOfInitialSolutions << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeMinGridResolution(TakeParameter);
    ss_TakeMinGridResolution >> min_grid_resolution;
    cout << dummyline << ":" << min_grid_resolution << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_TakeMaxGridResolution(TakeParameter);
    ss_TakeMaxGridResolution >> grid_resolution_increment;
    cout << dummyline << ":" << grid_resolution_increment << endl;

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

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_ActivateHGS(TakeParameter);
    ss_ActivateHGS >> ActivateHGS;
    cout << dummyline << ":" << ActivateHGS << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_Activate_Disturbance(TakeParameter);
    ss_Activate_Disturbance >> Activate_Disturbance;
    cout << dummyline << ":" << Activate_Disturbance << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_Theta(TakeParameter);
    ss_Theta >> theta;
    cout << dummyline << ":" << theta << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_Sigma(TakeParameter);
    ss_Sigma >> sigma;
    cout << dummyline << ":" << sigma << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_Phi(TakeParameter);
    ss_Phi >> phi;
    cout << dummyline << ":" << phi << endl;


    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_DisturbStrength(TakeParameter);
    ss_DisturbStrength >> DisturbStrength;
    cout << dummyline << ":" << DisturbStrength << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_MainAlgorithmTimeLimit(TakeParameter);
    ss_MainAlgorithmTimeLimit >> MainAlgorithmTimeLimit;
    cout << dummyline << ":" << MainAlgorithmTimeLimit << endl;

    getline(ifGlobal, dummyline);
    getline(ifGlobal, TakeParameter);
    stringstream ss_InitialSolutionMethod(TakeParameter);
    if (!(ss_InitialSolutionMethod >> InitialSolutionMethod))
    {
        cerr << "Error: Invalid InitialSolutionMethod in GlobalParameter.txt." << endl;
        exit(1);
    }
    cout << dummyline << ":" << InitialSolutionMethod << endl;

    //Inout parameter validation
    bool valid_input = true;
    if(OutputSolutionJSON != "YES" && OutputSolutionJSON != "NO")
    {
        cout << "Error: Invalid value for OutputSolutionJSON. Please set it to either 'YES' or 'NO'." << endl;
        valid_input = false;
    }
    if(NumberOfInitialSolutions <= 0)
    {
        cout << "Error: NumberOfInitialSolutions should be a positive integer." << endl;
        valid_input = false;
    }
    if(min_grid_resolution <= 0)
    {
        cout << "Error: min_grid_resolution should be a positive integer." << endl;
        valid_input = false;
    }
    if(grid_resolution_increment <= 0)
    {
        cout << "Error: grid_resolution_increment should be a positive integer." << endl;
        valid_input = false;
    }
    if(NumberOfExperiments <= 0)
    {
        cout << "Error: NumberOfExperiments should be a positive integer." << endl;
        valid_input = false;
    }
    if (InitialSolutionMethod != "ConstructAMMInitialSolution" &&
        InitialSolutionMethod != "Initial_BlockZone_Schedule")
    {
        cout << "Error: InitialSolutionMethod must be ConstructAMMInitialSolution or Initial_BlockZone_Schedule." << endl;
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
}

// Read one random seed per experiment from par/Seed.txt (one seed per line).
// The number of seeds must equal NumberOfExperiments, so call this after ReadGlobalParameter().
void file::ReadSeeds()
{
    cout << "Read Seeds" << endl;
    string SeedFile = MachineDirectory + "Curtin/IRPLR/IRPLR/Heuristic/par/Seed.txt";
    ifstream ifSeed(SeedFile.c_str());
    if (!ifSeed.is_open())
    {
        cout << "Error: Cannot open seed file " << SeedFile << endl;
        exit(1);
    }

    Seeds.clear();
    string line;
    int line_number = 0;
    while (getline(ifSeed, line))
    {
        line_number++;
        // Skip blank lines
        if (line.find_first_not_of(" \t\r\n") == string::npos)
        {
            continue;
        }
        stringstream ss_seed(line);
        long long seed;
        string trailing;
        if (!(ss_seed >> seed) || (ss_seed >> trailing) || seed < 0 || seed > numeric_limits<unsigned int>::max())
        {
            cout << "Error: Invalid seed on line " << line_number << " of Seed.txt: '" << line << "'" << endl;
            exit(1);
        }
        Seeds.push_back(static_cast<unsigned int>(seed));
    }

    if ((int)Seeds.size() != NumberOfExperiments)
    {
        cout << "Error: Seed.txt contains " << Seeds.size() << " seed(s) but NumberOfExperiments is " << NumberOfExperiments
             << ". The number of seeds must match the number of experiments." << endl;
        exit(1);
    }

    for (int i = 0; i < (int)Seeds.size(); i++)
    {
        cout << "Seed for experiment " << i + 1 << ":" << Seeds[i] << endl;
    }
}
