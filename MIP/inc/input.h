#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>

using namespace std;

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
  string InstanceName;
  vector<int> MinimumVisitDemand;
  depot Supplier;
  vector<customer> Retailers;

  transportation Vehicle;
  vector<vector<int>> Distance;
  vector<vector<int> > SortedRetailers;

  vector<vector<vector<int>>> AllPossibleSchedule; 

  void ReadIRPInstance(string &InstanceName, string &InstanceType, string &InstanceDirectories);
  void PrintData();
  double SupplierInitialInventory() const { return Supplier.InventoryBegin; }
  double SupplierProduction() const { return Supplier.QuantityProduced; }
  double CustomerInitialInventory(int customer) const { return Retailers[customer].InventoryBegin; }
  double CustomerMaximumInventory(int customer) const { return Retailers[customer].InventoryMax; }
  double CustomerDemand(int customer) const { return Retailers[customer].Demand; }
};

#endif
