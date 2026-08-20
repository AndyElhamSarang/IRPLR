#include "lib.h"
void file::ReadGlobalParameter()
{
    cout << "Read Global Parameter" << endl;
    string GlbalSetting = MachineDirectory + "Curtin/IRPLR/IRPLR/MIP/par/GlobalSetting.txt";
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

}