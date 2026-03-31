#include "lib.h"

int solution_improvement::OperatorTransfer(
    input &IRPLR,
    solution &IRPSolution,
    double &PenaltyForStockOut,
    vector<vector<int>> &TransferDetails)
{ // Current implementation only permits at most one customer therefore

    int TransferNumber = 1;
    assert(TransferNumber <= 1);
    assert(TransferNumber <= 1);

    cout << "Transfer starting solution" << endl;
    int whether_improved_or_not = 0;
    double LR_objv = numeric_limits<double>::max();
    IRPSolution.GetLogisticRatio(IRPLR);
    IRPSolution.print_solution(IRPLR);
    // cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut" << IRPSolution.ViolationStockOut << endl;
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut);
    // cout << "LR objv:" << LR_objv << endl;

    vector<vector<double>> ImpDeliveryQuantityCustomerTransfer;
    vector<vector<double>> ImpInventoryLevelCustomerTransfer;
    vector<vector<double>> ImpVehicleLoad;
    double ImpStockOut = 0;
    double ImpLogisticRatio = 0;
    double ImpTotalTransportationCost = 0;
    double ImpTotalDelivery = 0;

    int solutionCounter = 0;
    int naive_implementation = 0;
    try
    {

        for (int i = 0; i < TransferDetails.size(); i++)
        {
            int pick_day_removed = TransferDetails[i][1];
            int pick_vehicle_added = TransferDetails[i][2];

            time(&LS_end_time);
            double total_ls_time = difftime(LS_end_time, LS_start_time);
            if (total_ls_time - LocalSearchTimeLimit > 0.00001)
            {
                int time_limit_reached = total_ls_time;
                throw time_limit_reached;
            }
            ////////////////////////////////////////////////////////////////////////////////
            //                                                                            //
            //      Efficient implementation of evaluating the swap remove insert move    //
            //                                                                            //
            ////////////////////////////////////////////////////////////////////////////////
            solutionCounter++;
            /////////////////////////////////////////////////////////////////////////////////
            // Initialize all data needed to capture the improving solution
            vector<double> NewDeliveryQuantityCustomerTransfer(IRPSolution.DeliveryQuantity[TransferDetails[i][0]]);
            vector<double> NewInventoryLevelCustomerTransfer(IRPSolution.InventoryLevel[TransferDetails[i][0]]);
            double NewStockOut = IRPSolution.ViolationStockOut;
            double ChangeInTotalQuantity = 0;
            vector<vector<double>> CopyVehicleLoad = IRPSolution.VehicleLoad;
            cout << "================================================================" << endl;
            cout << "TransferDetails: Retailer " << TransferDetails[i][0] << ", from day " << TransferDetails[i][1] << " to day " << TransferDetails[i][2] << endl;
            cout << "NewDeliveryQuantityCustomerTransfer:";
            for (int period = 0; period < NewDeliveryQuantityCustomerTransfer.size(); period++)
            {
                cout << NewDeliveryQuantityCustomerTransfer[period] << ",";
            }
            cout << endl;

            cout << "NewInventoryLevelCustomerTransfer:";
            for (int period = 0; period < NewInventoryLevelCustomerTransfer.size(); period++)
            {
                cout << NewInventoryLevelCustomerTransfer[period] << ",";
            }
            cout << endl;

            /////////////////////////////////////////////////////////////////////////////////
            // Get change in delivery quantity and inventory level for the removed customers

            CopyVehicleLoad[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]] = CopyVehicleLoad[TransferDetails[i][1]][IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]]] - IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][1]];
            NewDeliveryQuantityCustomerTransfer[TransferDetails[i][1]] = 0;
            ChangeInTotalQuantity = ChangeInTotalQuantity - IRPSolution.DeliveryQuantity[TransferDetails[i][0]][TransferDetails[i][1]];

            // The following code adjust the delivery quantity and inventory level of the removed customer across all affected periods.
            if (TransferDetails[i][1] == 0)
            {
                AdjustQuantityAndInventoryLevel(
                    IRPLR.Retailers[TransferDetails[i][0]].InventoryBegin,
                    TransferDetails[i][1],
                    IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    TransferDetails[i][0],
                    IRPLR);
            }
            else
            {
                AdjustQuantityAndInventoryLevel(
                    IRPSolution.InventoryLevel[TransferDetails[i][0]][TransferDetails[i][1] - 1],
                    TransferDetails[i][1],
                    IRPSolution.VehicleAllocation[TransferDetails[i][0]][TransferDetails[i][1]],
                    NewDeliveryQuantityCustomerTransfer,
                    NewInventoryLevelCustomerTransfer,
                    CopyVehicleLoad,
                    IRPSolution.VehicleAllocation,
                    ChangeInTotalQuantity,
                    NewStockOut,
                    TransferDetails[i][0],
                    IRPLR);
            }
            cout << "--------------------------------------------------------------------------------" << endl;
            cout << "After removing the customer from the original day, the change in total quantity is: " << ChangeInTotalQuantity << ", and the new stock out is: " << NewStockOut << endl;

            cout << "NewDeliveryQuantityCustomerTransfer:";
            for (int period = 0; period < NewDeliveryQuantityCustomerTransfer.size(); period++)
            {
                cout << NewDeliveryQuantityCustomerTransfer[period] << ",";
            }
            cout << endl;

            cout << "NewInventoryLevelCustomerTransfer:";
            for (int period = 0; period < NewInventoryLevelCustomerTransfer.size(); period++)
            {
                cout << NewInventoryLevelCustomerTransfer[period] << ",";
            }
            cout << endl;
            // cout << "SelectedVehicleLoad: " << SelectedVehicleLoad << ", ChangeInTotalQuantity: " << ChangeInTotalQuantity << endl;
            // cout << "NewDeliveryQuantityCustomerTransfer" << endl;
            // for (int i = 0; i < NewDeliveryQuantityCustomerTransfer.size(); i++)
            // {
            //     for (int j = 0; j < NewDeliveryQuantityCustomerTransfer[i].size(); j++)
            //     {
            //         cout << NewDeliveryQuantityCustomerTransfer[i][j] << ",";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
            // cout << "NewInventoryLevelCustomerTransfer" << endl;
            // for (int i = 0; i < NewInventoryLevelCustomerTransfer.size(); i++)
            // {
            //     for (int j = 0; j < NewInventoryLevelCustomerTransfer[i].size(); j++)
            //     {
            //         cout << NewInventoryLevelCustomerTransfer[i][j] << ",";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
            // cout << "NewDeliveryQuantityCustomerInsert" << endl;
            // for (int i = 0; i < NewDeliveryQuantityCustomerInsert.size(); i++)
            // {
            //     for (int j = 0; j < NewDeliveryQuantityCustomerInsert[i].size(); j++)
            //     {
            //         cout << NewDeliveryQuantityCustomerInsert[i][j] << ",";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
            // cout << "NewInventoryLevelCustomerInsert" << endl;
            // for (int i = 0; i < NewInventoryLevelCustomerInsert.size(); i++)
            // {
            //     for (int j = 0; j < NewInventoryLevelCustomerInsert[i].size(); j++)
            //     {
            //         cout << NewInventoryLevelCustomerInsert[i][j] << ",";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
        }
    }
    catch (int time_limit_reached)
    {
        cout << "!Stop by time limit" << endl;
    }
    // cout << "Total solution explored:" << solutionCounter << endl;
    // cout << "whether_improved_or_not:" << whether_improved_or_not << endl;
    // if (whether_improved_or_not == 1)
    // {
    //     IRPSolution.Route = ImpRoute;
    //     IRPSolution.InventoryLevel = ImpInventoryLevel;
    //     IRPSolution.DeliveryQuantity = ImpDeliveryQuantity;
    //     IRPSolution.UnallocatedCustomers = ImpUnallocatedCustomers;
    //     IRPSolution.VehicleAllocation = ImpVehicleAllocation;
    //     IRPSolution.VehicleLoad = ImpVehicleLoad;
    //     IRPSolution.VisitOrder = ImpVisitOrder;
    //     IRPSolution.ViolationStockOut = ImpStockOut;

    //     IRPSolution.GetLogisticRatio(IRPLR);
    //     assert(fabs(IRPSolution.ViolationStockOut - ImpStockOut) < 0.00001);

    //     // Update the visit order
    //     for (int i = 0; i < IRPSolution.VisitOrder.size(); i++)
    //     {
    //         for (int j = 0; j < IRPSolution.VisitOrder[i].size(); j++)
    //         {
    //             IRPSolution.VehicleAllocation[i][j] = IRPLR.NumberOfVehicles + 1;
    //             IRPSolution.VisitOrder[i][j] = IRPLR.Retailers.size() + 1;
    //         }
    //     }

    //     IRPSolution.UnallocatedCustomers.clear();
    //     for (int i = 0; i < IRPSolution.Route.size(); i++) // For this time period
    //     {
    //         vector<int> TempUnallocatedCustomer;             // Look for unallcated customers at this time period
    //         for (int x = 0; x < IRPLR.Retailers.size(); x++) // Check each retailers
    //         {
    //             int UnallocatedYesOrNo = 0;
    //             for (int j = 0; j < IRPSolution.Route[i].size(); j++) // index j for vehicle
    //             {
    //                 for (int k = 0; k < IRPSolution.Route[i][j].size(); k++) // index k for position
    //                 {

    //                     if (IRPSolution.Route[i][j][k] == x)
    //                     {
    //                         UnallocatedYesOrNo = 1;
    //                         IRPSolution.VehicleAllocation[x][i] = j;
    //                         IRPSolution.VisitOrder[x][i] = k;
    //                     }
    //                 }
    //             }
    //             if (UnallocatedYesOrNo == 0) // This customer is not visited
    //             {
    //                 TempUnallocatedCustomer.push_back(x);
    //             }
    //         }
    //         IRPSolution.UnallocatedCustomers.push_back(TempUnallocatedCustomer);
    //     }
    // }
    return whether_improved_or_not;
}