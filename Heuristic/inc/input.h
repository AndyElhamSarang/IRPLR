class input
{
private:
  struct depot
  {
    int ID;
    double xCoord;
    double yCoord;
    double InventoryBegin;
    double QuantityProduced;
    double InventoryCost;
  };

  struct customer
  {
    int ID;
    double xCoord;
    double yCoord;
    double InventoryBegin;
    double InventoryMax;
    double InventoryMin;
    double Demand;
    double InventoryCost;
  };

  struct transportation
  {
    double capacity;
  };

public:
  int NumberOfRetailers;
  int TimeHorizon;
  int NumberOfVehicles;
  double VehiclesTotalCapacity;
  depot Supplier;
  vector<customer> Retailers;

  transportation Vehicle;
  vector<vector<double>> Distance;
  vector<vector<int> > SortedRetailers;

  vector<vector<vector<int>>> AllPossibleSchedule; 

  void ReadIRPInstance(string &InstanceName, string &InstanceType, string &InstanceDirectories);
  void PrintData();
};
