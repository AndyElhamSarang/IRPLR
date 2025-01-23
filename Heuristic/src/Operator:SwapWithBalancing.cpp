#include "lib.h"
int solution_improvement::OperatorSwapWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory)
{
    cout << "Swap starting solution" << endl;
    int whether_improved_or_not = 0;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    LR_objv = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    cout << "LR objv:" << LR_objv << endl;

    vector<vector<vector<int>>> ImpRoute(IRPSolution.Route);
    vector<vector<int>> ImpUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
    vector<vector<double>> ImpVehicleLoad(IRPSolution.VehicleLoad);
    vector<vector<double>> ImpDeliveryQuantity(IRPSolution.DeliveryQuantity);
    vector<vector<double>> ImpInventoryLevel(IRPSolution.InventoryLevel);
    vector<vector<int>> ImpVehicleAllocation(IRPSolution.VehicleAllocation);
    double ImpStockOut = 0;
    int solutionCounter = 0;
    for (int i = 0; i < IRPSolution.Route.size(); i++)
    {
        for (int j = 0; j < IRPSolution.Route[i].size(); j++)
        {
            for (int k = 0; k < IRPSolution.Route[i][j].size(); k++)
            {
                for (int x = 0; x < IRPSolution.UnallocatedCustomers[i].size(); x++)
                {

                    double ChangeInTransportationCost = 0;
                    double ChangeInTotalQuantity = 0;
                    double NewStockOut = IRPSolution.ViolationStockOut;
                    cout << "solution: " << solutionCounter << endl;
                    vector<vector<vector<int>>> TempRoute(IRPSolution.Route);
                    vector<vector<int>> TempUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
                    vector<vector<double>> TempVehicleLoad(IRPSolution.VehicleLoad);
                    vector<vector<double>> TempDeliveryQuantity(IRPSolution.DeliveryQuantity);
                    vector<vector<double>> TempInventoryLevel(IRPSolution.InventoryLevel);
                    vector<vector<int>> TempVehicleAllocation(IRPSolution.VehicleAllocation);

                    int tempCustomer = TempRoute[i][j][k];
                    TempRoute[i][j][k] = TempUnallocatedCustomers[i][x];
                    TempUnallocatedCustomers[i][x] = tempCustomer;
                    PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);

                    if (k == 0)
                    {
                        // cout << "From depot" << endl;
                        if (TempRoute[i][j].size() <= 1)
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                        }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                        }
                    }
                    else if (k == IRPSolution.Route[i][j].size() - 1)
                    {
                        // cout << "Back to depot" << endl;
                        if (TempRoute[i][j].size() <= 1)
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                        }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                        }
                    }
                    else
                    {
                        ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                    }

                    TempVehicleAllocation[TempRoute[i][j][k]][i] = TempVehicleAllocation[tempCustomer][i];
                    TempVehicleAllocation[tempCustomer][i] = IRPLR.NumberOfVehicles + 1;

                    int BalancingCounter = 0;
                    while (BalancingCounter < 10)
                    {
                        cout<<"Balancing attempt:"<<BalancingCounter<<endl;
                        double temp_LR_obvj = OperatorBalancing(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                        BalancingCounter++;
                    }

                    // cout << "Starting route:" << endl;
                    // for (int y = 0; y < TempRoute[i][j].size(); y++)
                    //{
                    // cout << TempRoute[i][j][y] << ",";
                    // }
                    // cout << endl;
                    double CurrentTransportationCost = memory.PopulateSingleRoutePrefixAndSuffix(IRPLR, TempRoute[i][j]); // Preprocessing travel distance of the route
                    memory.PopulateSingleRouteSubpath(IRPLR, TempRoute[i][j]);
                    double ImprovedTransportationCost = CurrentTransportationCost;
                    int FindingCheapestInsertion = OperatorCheapestInsertion(IRPLR, TempRoute[i][j], k, PenaltyForStockOut, ImprovedTransportationCost, memory); // Finding cheapest insertion
                    ChangeInTransportationCost += ImprovedTransportationCost - CurrentTransportationCost;
                    double NewTransportationCost = IRPSolution.TotalTransportationCost + ChangeInTransportationCost;
                    double NewTotalDelivery = IRPSolution.TotalDelivery + ChangeInTotalQuantity;

                    // cout << "Route resulting in cheapest insertion:" << endl;
                    // for (int y = 0; y < TempRoute[i][j].size(); y++)
                    //{
                    //     cout << TempRoute[i][j][y] << ",";
                    // }
                    // cout << endl;

                    double NewLogisticRatio = NewTransportationCost / NewTotalDelivery;

                    // double TempTransportationCost = 0;
                    // double TempTotalDelivery = 0;
                    // double TempLogisticRatio = 0;
                    // double TempViolationStockOut = 0;
                    // GetTempLogisticRatio(IRPLR, TempRoute, TempDeliveryQuantity, TempInventoryLevel, TempTransportationCost, TempTotalDelivery, TempLogisticRatio, TempViolationStockOut);
                    // cout << "Final solution" << endl;
                    // PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation);
                    // cout << "Check Transportation Cost" << endl;
                    // cout << TempTransportationCost << "," << NewTransportationCost << endl;
                    // assert(fabs(TempTransportationCost - NewTransportationCost) < 0.001);
                    // cout << "Check Delivery Quantity" << endl;
                    // cout << TempTotalDelivery << "," << NewTotalDelivery << endl;
                    // assert(fabs(TempTotalDelivery - NewTotalDelivery) < 0.001);
                    // cout << "Check Logistic ratio" << endl;
                    // cout << TempLogisticRatio << "," << NewLogisticRatio << endl;
                    // assert(fabs(TempLogisticRatio - NewLogisticRatio) < 0.001);
                    // cout << "Check ViolationStockout" << endl;
                    // cout << TempViolationStockOut << "," << NewStockOut << endl;
                    // assert(fabs(TempViolationStockOut - NewStockOut) < 0.001);
                    //  cout << temp_LR_obvj << "," << LR_objv << endl;
                    /*if (temp_LR_obvj < LR_objv)
                    {
                        whether_improved_or_not = 1;
                        ImpRoute = TempRoute;
                        ImpInventoryLevel = TempInventoryLevel;
                        ImpDeliveryQuantity = TempDeliveryQuantity;
                        ImpUnallocatedCustomers = TempUnallocatedCustomers;
                        ImpVehicleAllocation = TempVehicleAllocation;
                        ImpVehicleLoad = TempVehicleLoad;
                        if (NewStockOut > 0)
                        {

                            ImpStockOut = NewStockOut;
                        }
                        else
                        {
                            ImpStockOut = 0;
                            // cout << "!Improving feasible solution found" << endl;
                        }

                        // cout << "!Improving solution found" << endl;
                    }*/

                    solutionCounter++;
                }
            }
        }
    }
    cout << "Total solution explored:" << solutionCounter << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    if (whether_improved_or_not == 1)
    {
        IRPSolution.Route = ImpRoute;
        IRPSolution.InventoryLevel = ImpInventoryLevel;
        IRPSolution.DeliveryQuantity = ImpDeliveryQuantity;
        IRPSolution.UnallocatedCustomers = ImpUnallocatedCustomers;
        IRPSolution.VehicleAllocation = ImpVehicleAllocation;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.ViolationStockOut = ImpStockOut;

        IRPSolution.GetLogisticRatio(IRPLR);
        assert(fabs(IRPSolution.ViolationStockOut - ImpStockOut) < 0.001);

        cout << "Solution before rebalancing" << endl;
        IRPSolution.print_solution(IRPLR);

        // IRPSolution.GetLogisticRatio(IRPLR);
        // cout << IRPSolution.ViolationStockOut << "," << violation << endl;
        // assert(fabs(IRPSolution.ViolationStockOut - violation) < 0.001);
    }

    return whether_improved_or_not;
}