#include "lib.h"
void file::ReadIRPInstanceName()
{

  for (int i = 0; i < InstanceDirectories.size(); i++)
  {
    vector<string> temp_Instances;
    string InstanceSource = MachineDirectory + InstanceDirectories[i] + InstanceFile;
    cout << InstanceSource << endl;
    ifstream InputInstanceSource;
    InputInstanceSource.open(InstanceSource.c_str());
    if (!InputInstanceSource.is_open())
    {
      exit(EXIT_FAILURE);
    }
    string tempInstanceName;
    string InstanceName;
    InputInstanceSource >> tempInstanceName;
    InstanceName = MachineDirectory + InstanceDirectories[i] + tempInstanceName;

    while (!InputInstanceSource.eof())
    {
      temp_Instances.push_back(InstanceName);
      InputInstanceSource >> tempInstanceName;
      InstanceName = MachineDirectory + InstanceDirectories[i] + tempInstanceName;
    }
    instances.push_back(temp_Instances);
  }
}
