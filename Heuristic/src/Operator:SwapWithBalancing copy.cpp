#include "lib.h"
int solution_improvement::OperatorSwapWithBalancing(input &IRPLR, solution &IRPSolution, HGS &Routing, double &PenaltyForStockOut, preprocessing &memory)
{
    //This operator permits feasible solutions only

    cout << "Swap starting solution" << endl;
    int min_sequence_length_1=0;
    int max_sequence_length_1=1;
    int min_sequence_length_2=0;
    int max_sequence_length_2=1;

    int whether_improved_or_not = 0;
    int CountingInfeasibleCase = 0;
    double CurrentLogisticRatio = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    CurrentLogisticRatio = IRPSolution.LogisticRatio + PenaltyForStockOut * IRPSolution.ViolationStockOut;
    cout << "CurrentLogisticRatio :" << CurrentLogisticRatio << endl;

    vector<vector<vector<int>>> ImpRoute(IRPSolution.Route);
    vector<vector<int>> ImpUnallocatedCustomers(IRPSolution.UnallocatedCustomers);
    vector<vector<double>> ImpVehicleLoad(IRPSolution.VehicleLoad);
    vector<vector<double>> ImpDeliveryQuantity(IRPSolution.DeliveryQuantity);
    vector<vector<double>> ImpInventoryLevel(IRPSolution.InventoryLevel);
    vector<vector<int>> ImpVehicleAllocation(IRPSolution.VehicleAllocation);
    vector<vector<int>> ImpVisitOrder(IRPSolution.VisitOrder);
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
                    for (int sequence_length_1 = min_sequence_length_1; sequence_length_1 <= max_sequence_length_1; sequence_length_1++)
                    {
                        for (int sequence_length_2 = min_sequence_length_2; sequence_length_2 <= max_sequence_length_2; sequence_length_2++)
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
                            vector<vector<int>> TempVisitOrder(IRPSolution.VisitOrder);

                            int tempCustomer = TempRoute[i][j][k];
                            TempRoute[i][j][k] = TempUnallocatedCustomers[i][x];
                            TempUnallocatedCustomers[i][x] = tempCustomer;
                            PrintTempSolution(IRPLR, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder);

                            TempVehicleAllocation[TempRoute[i][j][k]][i] = TempVehicleAllocation[tempCustomer][i];
                            TempVisitOrder[TempRoute[i][j][k]][i] = TempVisitOrder[tempCustomer][i];
                            TempVehicleAllocation[tempCustomer][i] = IRPLR.NumberOfVehicles + 1;
                            TempVisitOrder[tempCustomer][i] = IRPLR.Retailers.size() + 1;
                            int BalancingCounter = 0;
                            int FeasibleRebalanceOrNot = 0;
                            double NewLogisticRatio = 0;
                            while (BalancingCounter < 1)
                            {
                                cout << "Balancing attempt:" << BalancingCounter << endl;
                                NewLogisticRatio = OperatorBalancing(IRPLR, memory, TempRoute, TempUnallocatedCustomers, TempVehicleLoad, TempDeliveryQuantity, TempInventoryLevel, TempVehicleAllocation, TempVisitOrder, CountingInfeasibleCase, FeasibleRebalanceOrNot);
                                BalancingCounter++;
                            }
                            if (FeasibleRebalanceOrNot == 1)
                            {
                                cout << "NewLogisticRatio: " << NewLogisticRatio << ", CurrentLogisticRatio: " << CurrentLogisticRatio << endl;
                                if (NewLogisticRatio < CurrentLogisticRatio)
                                {
                                    whether_improved_or_not = 1;
                                    ImpRoute = TempRoute;
                                    ImpUnallocatedCustomers = TempUnallocatedCustomers;
                                    ImpVehicleLoad = TempVehicleLoad;
                                    ImpDeliveryQuantity = TempDeliveryQuantity;
                                    ImpInventoryLevel = TempInventoryLevel;
                                    ImpVehicleAllocation = TempVehicleAllocation;
                                    ImpVisitOrder = TempVisitOrder;
                                }
                            }
                            solutionCounter++;
                        }
                    }
                }
            }
        }
    }
    cout << "Total # of solutions explored:" << solutionCounter << endl;
    cout << "Total # of solutions which are infeasible by balancing:" << CountingInfeasibleCase << endl;
    cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    if (whether_improved_or_not == 1)
    {
        IRPSolution.Route = ImpRoute;
        IRPSolution.UnallocatedCustomers = ImpUnallocatedCustomers;
        IRPSolution.VehicleLoad = ImpVehicleLoad;
        IRPSolution.DeliveryQuantity = ImpDeliveryQuantity;
        IRPSolution.InventoryLevel = ImpInventoryLevel;
        IRPSolution.VehicleAllocation = ImpVehicleAllocation;
        IRPSolution.VisitOrder = ImpVisitOrder;
        IRPSolution.ViolationStockOut = ImpStockOut;

        IRPSolution.GetLogisticRatio(IRPLR);
        assert(fabs(IRPSolution.ViolationStockOut - ImpStockOut) < 0.001);
    }

    return whether_improved_or_not;
}