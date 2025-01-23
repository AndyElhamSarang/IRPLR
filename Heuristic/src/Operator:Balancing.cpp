#include "lib.h"
double solution_improvement::OperatorBalancing(input &IRPLR,
                                               vector<vector<vector<int>>> &Route,
                                               vector<vector<int>> &UnallocatedCustomers,
                                               vector<vector<double>> &VehicleLoad,
                                               vector<vector<double>> &DeliveryQuantity,
                                               vector<vector<double>> &InventoryLevel,
                                               vector<vector<int>> &VehicleAllocation)
{
    cout << "Balancing quantity operator" << endl;
    double objv = 0;
    vector<int> AllCustomers;
    for (int i = 0; i < VehicleLoad.size(); i++)
    {
        for (int j = 0; j < VehicleLoad[i].size(); j++)
        {
            VehicleLoad[i][j] = 0;
        }
    }
    for (int i = 0; i < DeliveryQuantity.size(); i++)
    {
        for (int j = 0; j < DeliveryQuantity[i].size(); j++)
        {
            DeliveryQuantity[i][j] = 0;
            InventoryLevel[i][j] = 0;
        }
        AllCustomers.push_back(i);
    }
    
    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    boost_random_mechanism RandomnessInBalance;
    assert(AllCustomers.size() != 0);

    for (int i = 0; i < IRPLR.TimeHorizon; i++)
    {
        vector<int> PotentialCustomers(AllCustomers);
        while (PotentialCustomers.size() != 0)
        {
            int RandomCustomer = RandomnessInBalance.random_number_generator(0, PotentialCustomers.size() - 1, generator);
            if (VehicleAllocation[PotentialCustomers[RandomCustomer]][i] != IRPLR.NumberOfVehicles + 1) // if this customer is visited on this day
            {
                DeliveryQuantity[PotentialCustomers[RandomCustomer]][i] =
                    min(IRPLR.Vehicle.capacity - VehicleLoad[i][VehicleAllocation[PotentialCustomers[RandomCustomer]][i]],
                        IRPLR.Retailers[PotentialCustomers[RandomCustomer]].InventoryMax - InventoryLevel[PotentialCustomers[RandomCustomer]][i]);
                PotentialCustomers.erase(PotentialCustomers.begin() + RandomCustomer, PotentialCustomers.begin() + RandomCustomer + 1);
            }
            else
            {
                PotentialCustomers.erase(PotentialCustomers.begin() + RandomCustomer, PotentialCustomers.begin() + RandomCustomer + 1);
            }
        }
        //         if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
        //         {
        //             IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
        //             vehicle_index = i;
        //         }
    }
    cout << "End balancing" << endl;
    PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation);
    
    // if (LookBackwardPeriod > 0)
    // {
    //     double Load = 0;
    //     int vehicle_index = IRPLR.NumberOfVehicles;
    //     for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
    //     {
    //         Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod - 1]);
    //         if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
    //         {
    //             IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
    //             vehicle_index = i;
    //         }
    //     }
    //     IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
    //     IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
    //     IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
    // }
    // else
    // {
    //     double Load = 0;
    //     int vehicle_index = IRPLR.NumberOfVehicles;
    //     for (int i = 0; i < IRPLR.NumberOfVehicles; i++)
    //     {
    //         Load = min(IRPLR.Vehicle.capacity - IRPSolution.VehicleLoad[LookBackwardPeriod][i], IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryMax - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin);
    //         if (IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] < Load)
    //         {
    //             IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = Load;
    //             vehicle_index = i;
    //         }
    //     }
    //     IRPSolution.Route[LookBackwardPeriod][vehicle_index].push_back(CandidateRetailers[RandomPickARetailer]);
    //     IRPSolution.VehicleLoad[LookBackwardPeriod][vehicle_index] += IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod];
    //     IRPSolution.VehicleAllocation[CandidateRetailers[RandomPickARetailer]][LookBackwardPeriod] = vehicle_index;
    // }
    // int tempInventory = IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].InventoryBegin;
    // for (int i = 0; i < IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]].size(); i++)
    // {
    //     tempInventory = tempInventory - IRPLR.Retailers[CandidateRetailers[RandomPickARetailer]].Demand + IRPSolution.DeliveryQuantity[CandidateRetailers[RandomPickARetailer]][i];
    //     IRPSolution.InventoryLevel[CandidateRetailers[RandomPickARetailer]][i] = tempInventory;
    // }
    cout << "Exit balancing" << endl;
    return objv;
}