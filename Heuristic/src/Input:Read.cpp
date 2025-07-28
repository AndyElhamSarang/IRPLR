#include "lib.h"

void input::ReadIRPInstance(string &InstanceName, string &InstanceType, string &InstanceDirectories)
{
    // cout << "Read instance data" << endl;
    Retailers.clear();
    Distance.clear();
    if (InstanceType == "Single vehicle IRP")
        NumberOfVehicles = 1;
    else if (InstanceType == "Multiple vehicles IRP")
    {
        string ExtractNumberOfVehicles = InstanceName.substr(MachineDirectory.size() + InstanceDirectories.size(), InstanceName.size());
        cout << "Instance's name: " << ExtractNumberOfVehicles << endl;
        string ExtractNumberOfVehicles_step2 = ExtractNumberOfVehicles.substr(3, ExtractNumberOfVehicles.size());
        // cout<<"ExtractNumberOfVehicles_step2:"<<ExtractNumberOfVehicles_step2<<endl;
        stringstream ss_take_NumberOfVehicle(ExtractNumberOfVehicles_step2);
        int DummyInteger = 0;
        char DummyChar;
        ss_take_NumberOfVehicle >> DummyInteger >> DummyChar >> DummyInteger >> DummyChar >> NumberOfVehicles;
        NumberOfVehicles = NumberOfVehicles;
    }

    string takeline;
    char taketab;
    ifstream ifinstance;
    ifinstance.open(InstanceName.c_str());
    if (!ifinstance.is_open())
    {
        exit(EXIT_FAILURE);
    };
    getline(ifinstance, takeline);

    stringstream ss_take_line(takeline);

    while (ss_take_line >> NumberOfRetailers >> TimeHorizon >> Vehicle.capacity)
    {
        if (ss_take_line.peek() == '\t')
        {
            ss_take_line.ignore();
        }
    }
    // cout << NumberOfRetailers << '\t' << TimeHorizon << '\t' << Vehicle.capacity << endl;

    getline(ifinstance, takeline);

    stringstream ss_take_supplier(takeline);

    while (ss_take_supplier >> Supplier.ID >> Supplier.xCoord >> Supplier.yCoord >> Supplier.InventoryBegin >> Supplier.QuantityProduced >> Supplier.InventoryCost)
    {
        if (ss_take_supplier.peek() == '\t')
        {
            ss_take_supplier.ignore();
        }
    }

    // cout<< Supplier.ID<<'\t'<<Supplier.xCoord<<'\t'<<Supplier.yCoord<<'\t'<<Supplier.InventoryBegin << '\t' << Supplier.QuantityProduced<< '\t' << Supplier.InventoryCost << endl;

    while (getline(ifinstance, takeline))
    {
        stringstream ss_take_retailer(takeline);
        customer temp_retailer;
        while (ss_take_retailer >> temp_retailer.ID >> temp_retailer.xCoord >> temp_retailer.yCoord >> temp_retailer.InventoryBegin >> temp_retailer.InventoryMax >> temp_retailer.InventoryMin >> temp_retailer.Demand >> temp_retailer.InventoryCost)
        {
            if (ss_take_retailer.peek() == '\t')
            {
                ss_take_retailer.ignore();
            }
        }
        Retailers.push_back(temp_retailer);
    }
    /*for(int i=0;i<Retailers.size();i++)
    {
        cout<< Retailers[i].ID<<'\t'<<Retailers[i].xCoord<<'\t'<<Retailers[i].yCoord
        <<'\t'<<Retailers[i].InventoryBegin<<'\t'<<Retailers[i].InventoryMax<<'\t'<<
        Retailers[i].InventoryMin<<'\t'<<Retailers[i].Demand<<'\t'<<Retailers[i].InventoryCost<<endl;
    }*/

    // Calculate distance matrix
    vector<vector<double>> AllCoord;
    vector<double> temp_coord;
    temp_coord.push_back(Supplier.xCoord);
    temp_coord.push_back(Supplier.yCoord);
    AllCoord.push_back(temp_coord);
    for (int i = 0; i < Retailers.size(); i++)
    {
        temp_coord[0] = Retailers[i].xCoord;
        temp_coord[1] = Retailers[i].yCoord;
        AllCoord.push_back(temp_coord);
    }

    /*for (int i = 0; i < AllCoord.size(); i++)
    {
        cout << AllCoord[i][0] << "," << AllCoord[i][1] << endl;
    }*/
    for (int i = 0; i < AllCoord.size(); i++)
    {
        vector<double> temp_distance;
        for (int j = 0; j < AllCoord.size(); j++)
        {
            double travel = 0;
            if (i != j)
            {
                double diff_X = AllCoord[i][0] - AllCoord[j][0];
                double diff_Y = AllCoord[i][1] - AllCoord[j][1];
                travel = sqrt(pow(diff_X, power) + pow(diff_Y, power));
            }
            temp_distance.push_back(travel);
        }
        Distance.push_back(temp_distance);
    }
    /*for(int i=0;i<Distance.size();i++)
    {
        for(int j=0;j<Distance[i].size()-1;j++)
        {
            cout<<Distance[i][j]<<'\t';
        }
        cout<<Distance[i][Distance[i].size()-1]<<endl;
    }*/
    for (int i = 0; i < Retailers.size(); i++)
    {
        vector<int> tempSortedRetailers;
        for (int j = 0; j < Retailers.size(); j++)
        {
            if (j != i)
            {
                tempSortedRetailers.push_back(j);
            }
        }
        SortedRetailers.push_back(tempSortedRetailers);
    }
    for (int i = 0; i < SortedRetailers.size(); i++)
    {
        bool swap = true;
        while (swap == true)
        {
            swap = false;
            for (int j = 0; j < SortedRetailers[i].size() - 1; j++)
            {

                double DistanceToFirstRetailers = sqrt(pow(Retailers[i].xCoord - Retailers[SortedRetailers[i][j]].xCoord, power) + pow(Retailers[i].yCoord - Retailers[SortedRetailers[i][j]].yCoord, power));
                double DistanceToSecondRetailers = sqrt(pow(Retailers[i].xCoord - Retailers[SortedRetailers[i][j + 1]].xCoord, power) + pow(Retailers[i].yCoord - Retailers[SortedRetailers[i][j + 1]].yCoord, power));
                if (DistanceToFirstRetailers > DistanceToSecondRetailers)
                {
                    int tempRetailer = SortedRetailers[i][j];
                    SortedRetailers[i][j] = SortedRetailers[i][j + 1];
                    SortedRetailers[i][j + 1] = tempRetailer;
                    swap = true;
                }
            }
        }
    }
    if (printout_inputdata == 1)
    {
        cout << "SortedRetailers:" << endl;
        for (int i = 0; i < SortedRetailers.size(); i++)
        {
            for (int j = 0; j < SortedRetailers[i].size(); j++)
            {
                cout << SortedRetailers[i][j] << ",";
            }
            cout << endl;
        }
    }

    for(int i=0;i<Retailers.size();i++)
    {
        vector<int> DayVisitOrNot;
        for(int j=0;j<TimeHorizon;j++)
        {
            DayVisitOrNot.push_back(0);
        }

        /*for(int x=0;x<DayVisitOrNot.size();x++)
        {
            for
        }*/
    }

    /*for (int i = 0; i < Retailers.size(); i++)
    {
        int TotalSchedules = 0;
        TotalSchedules = pow(2, TimeHorizon);
        cout << TotalSchedules << endl;
        assert(AllPossibleSchedule[i].size() == TotalSchedules);
    }*/
    VehiclesTotalCapacity = Vehicle.capacity * NumberOfVehicles;
}