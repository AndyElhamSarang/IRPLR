#include "lib.h"
int solution_improvement::OperatorRepair(input &IRPLR, solution &IRPSolution, double &PenaltyForStockOut, double &PenaltyMoreThanCapacity)
{
    cout << "================================" << endl;
    cout << "OperatorRepair" << endl;
    bool repair_triggered = false;
    IRPSolution.GetLogisticRatio(IRPLR);
    double LR_objv = numeric_limits<double>::max();
    LR_objv = Calculate_la_relax_objv(IRPSolution.LogisticRatio, PenaltyForStockOut, IRPSolution.ViolationStockOut, PenaltyMoreThanCapacity, IRPSolution.ViolationMoreThanCapacity);
    cout << "TotalTransportationCost:" << IRPSolution.TotalTransportationCost << "\t TotalDelivery:" << IRPSolution.TotalDelivery << "\t LogistcRatio:" << IRPSolution.LogisticRatio << "\t ViolationStockOut: " << IRPSolution.ViolationStockOut << "\t PenaltyForStockOut:" << PenaltyForStockOut << "\t ViolationMoreThanCapacity:" << IRPSolution.ViolationMoreThanCapacity << "\t PenaltyMoreThanCapacity:" << PenaltyMoreThanCapacity << "\t LR_objv:" << LR_objv << endl;

    int whether_improved_or_not = 0;
    double improved_quantity_removal = 0;
    int improved_customer_index = IRPLR.NumberOfRetailers + 1;
    int improved_time = IRPLR.TimeHorizon + 1;
    int improved_vehicle = IRPLR.NumberOfVehicles + 1;
    for (int time = 0; time < IRPLR.TimeHorizon; time++)
    {
        for (int vehicle = 0; vehicle < IRPLR.NumberOfVehicles; vehicle++)
        {
            if (IRPSolution.VehicleLoad[time][vehicle] > IRPLR.Vehicle.capacity)
            {
                repair_triggered = true;
                double VehicleOverload = max(0.0, IRPSolution.VehicleLoad[time][vehicle] - IRPLR.Vehicle.capacity);
                IRPSolution.print_solution(IRPLR);
                cout << "Vehicle " << vehicle << " at time " << time << " is overloaded. VehicleLoad:" << IRPSolution.VehicleLoad[time][vehicle] << ", capacity:" << IRPLR.Vehicle.capacity << endl;

                cout << "Customer visited on this vehicle:";
                for (int customer = 0; customer < IRPSolution.Route[time][vehicle].size(); customer++)
                {
                    cout << IRPSolution.Route[time][vehicle][customer] << ",";
                }
                cout << endl;
                for (int customer = 0; customer < IRPSolution.Route[time][vehicle].size(); customer++)
                {

                    double InventoryBranchout = 0;
                    if (time == 0)
                    {
                        InventoryBranchout = IRPLR.Retailers[IRPSolution.Route[time][vehicle][customer]].InventoryBegin;
                    }
                    else
                    {
                        InventoryBranchout = IRPSolution.InventoryLevel[IRPSolution.Route[time][vehicle][customer]][time - 1];
                    }
                    double possible_quantity_removal = min(IRPSolution.InventoryLevel[IRPSolution.Route[time][vehicle][customer]][time], IRPSolution.DeliveryQuantity[IRPSolution.Route[time][vehicle][customer]][time]);
                    double LargestStockout = 0;
                    InventoryBranchout = InventoryBranchout - IRPLR.Retailers[IRPSolution.Route[time][vehicle][customer]].Demand + IRPSolution.DeliveryQuantity[IRPSolution.Route[time][vehicle][customer]][time] - possible_quantity_removal;
                    cout << InventoryBranchout << ",";
                    if (LargestStockout < -InventoryBranchout)
                    {
                        LargestStockout = -InventoryBranchout;
                    }
                    for (int Subsequent_Time = time + 1; Subsequent_Time < IRPSolution.DeliveryQuantity[IRPSolution.Route[time][vehicle][customer]].size(); Subsequent_Time++)
                    {
                        InventoryBranchout = InventoryBranchout - IRPLR.Retailers[IRPSolution.Route[time][vehicle][customer]].Demand + IRPSolution.DeliveryQuantity[IRPSolution.Route[time][vehicle][customer]][Subsequent_Time];
                        cout << InventoryBranchout << ",";
                        if (LargestStockout < -InventoryBranchout)
                        {
                            LargestStockout = -InventoryBranchout;
                        }
                    }
                    cout << endl;
                    possible_quantity_removal = max(0.0, possible_quantity_removal - LargestStockout);

                    double quantity_removal = min(possible_quantity_removal, VehicleOverload);
                    cout << "Customer " << IRPSolution.Route[time][vehicle][customer] << " at time " << time << ": possible quantity removal: " << possible_quantity_removal << "; vehicle overload: " << VehicleOverload << "; quantity to remove:" << quantity_removal << endl;

                    // cout << "After removing possible quantity removal, check inventory level feasibility." << endl;
                    // PrintTempSolution(IRPLR, Route, UnallocatedCustomers, VehicleLoad, DeliveryQuantity, InventoryLevel, VehicleAllocation, VisitOrder);

                    double temp_TotalDelivery = IRPSolution.TotalDelivery - quantity_removal;
                    double temp_LogisticRatio = (IRPSolution.TotalTransportationCost / temp_TotalDelivery);
                    double temp_ViolationStockOut = IRPSolution.ViolationStockOut;
                    double temp_ViolationMoreThanCapacity = IRPSolution.ViolationMoreThanCapacity - quantity_removal;

                    double temp_LR_objv = Calculate_la_relax_objv(temp_LogisticRatio, PenaltyForStockOut, temp_ViolationStockOut, PenaltyMoreThanCapacity, temp_ViolationMoreThanCapacity);
                    cout << "temp_TotalDelivery:" << temp_TotalDelivery << "; temp_LogisticRatio:" << temp_LogisticRatio << "; temp_ViolationStockOut:" << temp_ViolationStockOut << "; temp_ViolationMoreThanCapacity:" << temp_ViolationMoreThanCapacity << "; temp_LR_objv:" << temp_LR_objv << endl;
                    if (temp_LR_objv < LR_objv)
                    {
                        cout << "Improved solution found by removing quantity from customer " << IRPSolution.Route[time][vehicle][customer] << " at time " << time << ". Previous LR_objv: " << LR_objv << ", New LR_objv: " << temp_LR_objv << endl;
                        LR_objv = temp_LR_objv;
                        improved_quantity_removal = quantity_removal;
                        improved_customer_index = IRPSolution.Route[time][vehicle][customer];
                        improved_time = time;
                        improved_vehicle = vehicle;
                        whether_improved_or_not = 1;
                    }
                }
            }
        }
    }

    if (whether_improved_or_not == 1)
    {
        IRPSolution.DeliveryQuantity[improved_customer_index][improved_time] = IRPSolution.DeliveryQuantity[improved_customer_index][improved_time] - improved_quantity_removal;
        IRPSolution.InventoryLevel[improved_customer_index][improved_time] = IRPSolution.InventoryLevel[improved_customer_index][improved_time] - improved_quantity_removal;
        for (int Subsequent_Time = improved_time + 1; Subsequent_Time < IRPSolution.DeliveryQuantity[improved_customer_index].size(); Subsequent_Time++)
        {
            IRPSolution.InventoryLevel[improved_customer_index][Subsequent_Time] = IRPSolution.InventoryLevel[improved_customer_index][Subsequent_Time - 1] - IRPLR.Retailers[improved_customer_index].Demand + IRPSolution.DeliveryQuantity[improved_customer_index][Subsequent_Time];
        }
        IRPSolution.VehicleLoad[improved_time][IRPSolution.VehicleAllocation[improved_customer_index][improved_time]] = IRPSolution.VehicleLoad[improved_time][IRPSolution.VehicleAllocation[improved_customer_index][improved_time]] - improved_quantity_removal;

        IRPSolution.TotalDelivery = IRPSolution.TotalDelivery - improved_quantity_removal;
        IRPSolution.LogisticRatio = (IRPSolution.TotalTransportationCost / IRPSolution.TotalDelivery);
        IRPSolution.ViolationMoreThanCapacity = IRPSolution.ViolationMoreThanCapacity - improved_quantity_removal;
        double check_objv = IRPSolution.LogisticRatio;
        IRPSolution.print_solution(IRPLR);
        IRPSolution.GetLogisticRatio(IRPLR);
        assert(fabs(check_objv - IRPSolution.LogisticRatio) < 0.00001);

    }
// assert(repair_triggered == false);
    return whether_improved_or_not;
}