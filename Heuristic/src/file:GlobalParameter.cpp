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

    if (!ifGlobal)
    {
        cerr << "Error: cannot open " << GlobalParameter << endl;
        exit(1);
    }

    // The file is a sequence of pairs: a label line followed by a value line. The parameter is
    // identified by the label up to the first '|' (the rest of the label is a comment listing the
    // allowed values), so the entries may appear in any order.
    vector<pair<string, string>> Entries; // (name, value) in file order
    string LabelLine, ValueLine;
    auto Trim = [](const string &text)
    {
        const size_t first = text.find_first_not_of(" \t\r");
        if (first == string::npos)
            return string();
        const size_t last = text.find_last_not_of(" \t\r");
        return text.substr(first, last - first + 1);
    };
    while (getline(ifGlobal, LabelLine))
    {
        const string Name = Trim(LabelLine.substr(0, LabelLine.find('|')));
        if (Name.empty())
            continue; // blank line
        if (!getline(ifGlobal, ValueLine))
        {
            cerr << "Error: GlobalParameter.txt has no value line after \"" << Name << "\"." << endl;
            exit(1);
        }
        for (const auto &entry : Entries)
        {
            if (entry.first == Name)
            {
                cerr << "Error: " << Name << " appears twice in GlobalParameter.txt." << endl;
                exit(1);
            }
        }
        Entries.push_back({Name, Trim(ValueLine)});
    }

    // Each known parameter: (name, required, assign value). Listed in the order they are used in MSILS.
    string StringUseVehicleSlack = UseVehicleSlack ? "YES" : "NO";
    string StringDisturbance; // NO_DISTURB, SEGMENT or SHAW (YES/NO from older files mean SEGMENT/NO_DISTURB)
    struct Parameter
    {
        string name;
        bool required;
        function<bool(stringstream &)> assign;
    };
    const vector<Parameter> Parameters = {
        {"NumberOfExperiments", true, [&](stringstream &ss) { return bool(ss >> NumberOfExperiments); }},
        {"MainAlgorithmTimeLimit", true, [&](stringstream &ss) { return bool(ss >> MainAlgorithmTimeLimit); }},
        {"NumberOfInitialSolution", true, [&](stringstream &ss) { return bool(ss >> NumberOfInitialSolutions); }},
        {"InitialSolutionMethod", true, [&](stringstream &ss) { return bool(ss >> InitialSolutionMethod); }},
        {"MinGridResolution", true, [&](stringstream &ss) { return bool(ss >> min_grid_resolution); }},
        {"GridResolutionIncrement", true, [&](stringstream &ss) { return bool(ss >> grid_resolution_increment); }},
        {"Activate HGS", true, [&](stringstream &ss) { return bool(ss >> ActivateHGS); }},
        {"theta", true, [&](stringstream &ss) { return bool(ss >> theta); }},
        {"sigma", true, [&](stringstream &ss) { return bool(ss >> sigma); }},
        {"phi", true, [&](stringstream &ss) { return bool(ss >> phi); }},
        {"AllowLagrangianRelaxation", true, [&](stringstream &ss) { return bool(ss >> AllowLagrangianRelaxation); }},
        {"InitialLagrangianScalar", true, [&](stringstream &ss) { return bool(ss >> InitialLagrangianScalar); }},
        {"ToTriggerAdjustment", true, [&](stringstream &ss) { return bool(ss >> ToTriggerAdjustment); }},
        {"ToAdjustLagrangianScalar", true, [&](stringstream &ss) { return bool(ss >> ToAdjustLagrangianScalar); }},
        {"LocalSearchOrder", false, [&](stringstream &ss) { return bool(ss >> LocalSearchOrder); }},
        {"LocalSearchMemory", false, [&](stringstream &ss) { return bool(ss >> LocalSearchMemory); }},
        {"UseVehicleSlack", false, [&](stringstream &ss) { return bool(ss >> StringUseVehicleSlack); }},
        {"TypeOfRebalance", true, [&](stringstream &ss) { return bool(ss >> TypeOfRebalance); }},
        {"Activate_Disturbance", true, [&](stringstream &ss) { return bool(ss >> StringDisturbance); }},
        {"Disturb_strength", true, [&](stringstream &ss) { return bool(ss >> DisturbStrength); }},
    };

    bool read_ok = true;
    for (const auto &entry : Entries)
    {
        bool known = false;
        for (const auto &parameter : Parameters)
            known = known || parameter.name == entry.first;
        if (!known)
        {
            cerr << "Error: unknown parameter \"" << entry.first << "\" in GlobalParameter.txt." << endl;
            read_ok = false;
        }
    }
    for (const auto &parameter : Parameters)
    {
        const pair<string, string> *found = nullptr;
        for (const auto &entry : Entries)
            if (entry.first == parameter.name)
                found = &entry;
        if (found == nullptr)
        {
            if (parameter.required)
            {
                cerr << "Error: " << parameter.name << " is missing from GlobalParameter.txt." << endl;
                read_ok = false;
            }
            else
            {
                cout << parameter.name << " not given in GlobalParameter.txt, using default" << endl;
            }
            continue;
        }
        stringstream ss(found->second);
        string trailing;
        if (!parameter.assign(ss) || (ss >> trailing))
        {
            cerr << "Error: invalid value \"" << found->second << "\" for " << parameter.name << " in GlobalParameter.txt." << endl;
            read_ok = false;
            continue;
        }
        cout << parameter.name << ":" << found->second << endl;
    }
    if (!read_ok)
    {
        exit(1);
    }

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
    if (StringDisturbance == "NO_DISTURB" || StringDisturbance == "NO")
    {
        Activate_Disturbance = "NO";
    }
    else if (StringDisturbance == "SEGMENT" || StringDisturbance == "SHAW" || StringDisturbance == "YES")
    {
        Activate_Disturbance = "YES";
        DisturbOperator = (StringDisturbance == "SHAW") ? "SHAW" : "SEGMENT";
    }
    else
    {
        cout << "Error: Invalid value for Activate_Disturbance. Please set it to 'NO_DISTURB', 'SEGMENT' or 'SHAW'." << endl;
        valid_input = false;
    }
    if (LocalSearchOrder != "SEQUENTIAL" && LocalSearchOrder != "RVND")
    {
        cout << "Error: Invalid value for LocalSearchOrder. Please set it to either 'SEQUENTIAL' or 'RVND'." << endl;
        valid_input = false;
    }
    if (StringUseVehicleSlack != "YES" && StringUseVehicleSlack != "NO")
    {
        cout << "Error: Invalid value for UseVehicleSlack. Please set it to either 'YES' or 'NO'." << endl;
        valid_input = false;
    }
    UseVehicleSlack = (StringUseVehicleSlack == "YES");
    if (LocalSearchMemory != "NO" && LocalSearchMemory != "ROUTE" && LocalSearchMemory != "MIDDLE" && LocalSearchMemory != "EXACT")
    {
        cout << "Error: Invalid value for LocalSearchMemory. Please set it to 'NO', 'ROUTE', 'MIDDLE' or 'EXACT'." << endl;
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
