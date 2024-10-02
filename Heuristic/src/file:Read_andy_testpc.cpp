#include "lib.h"
void file::ReadDirectory()
{

    MachineDirectory = "/home/andytestpc/Desktop/";
    cout << "Read Parameter" << endl;
    string InputInstanceName = MachineDirectory + "Curtin/IRPLR/Heuristic/par/Instances.txt";
    ifstream ifinstance;
    ifinstance.open(InputInstanceName.c_str());
    getline(ifinstance,InstanceType);
    cout<<"InstanceType:"<<InstanceType<<endl;

    string TakeDummyLine;
    getline(ifinstance,TakeDummyLine);
    getline(ifinstance,InstanceFile);
    getline(ifinstance, TakeDummyLine);
    string InstanceDirectory;
    while (getline(ifinstance, InstanceDirectory))
        InstanceDirectories.push_back(InstanceDirectory);
        cout<<"Instance directories:"<<endl;
    for (int i = 0; i < InstanceDirectories.size(); i++)
        cout << InstanceDirectories[i] << endl;

    
}