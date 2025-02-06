CC=g++
andy = /home/andy/Desktop/Curtin/IRPLR/Heuristic/inc
andy_boost = /home/andy/Desktop/c++boost/boost_1_83_0
andy_desktop = /home/andy/Desktop/Curtin/IRPLR/Heuristic/inc
andy_desktop_boost = /home/andy/Desktop/c++boost/boost_1_83_0
andy_lenovo = /home/andy/Desktop/Curtin/IRPLR/Heuristic/inc
andy_lenovo_boost = /home/andy/Desktop/c++_boost/boost_1_83_0
andy_testpc = /home/andytestpc/Desktop/Curtin/IRPLR/Heuristic/inc
andy_testpc_boost = /home/andytestpc/Desktop/c++_boost/boost_1_83_0

andy:
	$(CC) main.cpp file:Read_andy.cpp file:ReadInstanceName.cpp file:GlobalParameter.cpp \
	Input:Read.cpp \
	PrintData.cpp random.cpp \
	DataStructure:Global.cpp DataStructure:CustomerInfo.cpp DataStructure:PrefixAndSuffix.cpp DataStructure:Local.cpp \
	DataStructure:Subpath.cpp \
	DataStructure:Concatenation.cpp \
	DataStructure:Print.cpp \
	HGS:ReadParameter.cpp HGS:Call.cpp \
	solution.cpp solution:initialize.cpp solution:GetLogisticRatio.cpp solution:print.cpp \
	solution:OutputCVRP.cpp solution:ReadCVRP_Solution.cpp \
	solution:GetTransCostSingleRoute.cpp \
	CheckStockOut.cpp \
	Initial:SingleVehicle.cpp Initial.cpp Initial:Scheduling.cpp  \
	InitialWithZone.cpp InitialScheduleWithCircleZone.cpp InitialScheduleWithBlockZone.cpp \
	LNS.cpp LNS:Destory.cpp LNS:Repair.cpp \
	Operator:PrintTempSolution.cpp Operator:GetTempLogisticRatio.cpp \
	Operator:SingleRoute.cpp Operator:Swap.cpp Operator:SwapWithBalancing.cpp \
	Operator:Repair.cpp \
	Operator:Insert.cpp Operator:Remove.cpp Operator:Balancing.cpp \
	-I $(andy) -I $(andy_boost) \
	-o irplr.out -O3
andy_lenovo:
	$(CC) main.cpp file:Read_andy_lenovo.cpp file:ReadInstanceName.cpp file:GlobalParameter.cpp \
	Input:Read.cpp \
	PrintData.cpp random.cpp \
	DataStructure:Global.cpp DataStructure:CustomerInfo.cpp DataStructure:PrefixAndSuffix.cpp DataStructure:Local.cpp \
	DataStructure:Subpath.cpp \
	DataStructure:Concatenation.cpp \
	DataStructure:Print.cpp \
	HGS:ReadParameter.cpp HGS:Call.cpp \
	solution.cpp solution:initialize.cpp solution:GetLogisticRatio.cpp solution:print.cpp \
	solution:OutputCVRP.cpp solution:ReadCVRP_Solution.cpp \
	solution:GetTransCostSingleRoute.cpp \
	CheckStockOut.cpp \
	Initial:SingleVehicle.cpp Initial.cpp Initial:Scheduling.cpp \
	InitialWithZone.cpp InitialScheduleWithCircleZone.cpp InitialScheduleWithBlockZone.cpp \
	LNS.cpp LNS:Destory.cpp LNS:Repair.cpp \
	Operator:PrintTempSolution.cpp Operator:GetTempLogisticRatio.cpp \
	Operator:SingleRoute.cpp Operator:Swap.cpp Operator:SwapWithBalancing.cpp \
	Operator:Repair.cpp \
	Operator:Insert.cpp Operator:Remove.cpp Operator:Balancing.cpp \
	-I $(andy_lenovo) -I $(andy_lenovo_boost) \
	-o irplr.out -O3
andy_desktop:
	$(CC) main.cpp file:Read_andy_desktop.cpp file:ReadInstanceName.cpp file:GlobalParameter.cpp \
	Input:Read.cpp \
	PrintData.cpp random.cpp \
	DataStructure:Global.cpp DataStructure:CustomerInfo.cpp DataStructure:PrefixAndSuffix.cpp DataStructure:Local.cpp \
	DataStructure:Subpath.cpp \
	DataStructure:Concatenation.cpp \
	DataStructure:Print.cpp \
	HGS:ReadParameter.cpp HGS:Call.cpp \
	solution.cpp solution:initialize.cpp solution:GetLogisticRatio.cpp solution:print.cpp \
	solution:OutputCVRP.cpp solution:ReadCVRP_Solution.cpp \
	solution:GetTransCostSingleRoute.cpp \
	CheckStockOut.cpp \
	Initial:SingleVehicle.cpp Initial.cpp Initial:Scheduling.cpp \
	InitialWithZone.cpp InitialScheduleWithCircleZone.cpp InitialScheduleWithBlockZone.cpp \
	LNS.cpp LNS:Destory.cpp LNS:Repair.cpp \
	Operator:PrintTempSolution.cpp Operator:GetTempLogisticRatio.cpp \
	Operator:SingleRoute.cpp Operator:Swap.cpp Operator:SwapWithBalancing.cpp \
	Operator:Repair.cpp \
	Operator:Insert.cpp Operator:Remove.cpp Operator:Balancing.cpp \
	-I $(andy_desktop) -I $(andy_desktop_boost) \
	-o test.out
andy_testpc:
	$(CC) main.cpp file:Read_andy_testpc.cpp file:ReadInstanceName.cpp file:GlobalParameter.cpp \
	Input:Read.cpp \
	PrintData.cpp random.cpp \
	DataStructure:Global.cpp DataStructure:CustomerInfo.cpp DataStructure:PrefixAndSuffix.cpp DataStructure:Local.cpp \
	DataStructure:Subpath.cpp \
	DataStructure:Concatenation.cpp \
	DataStructure:Print.cpp \
	HGS:ReadParameter.cpp HGS:Call.cpp \
	solution.cpp solution:initialize.cpp solution:GetLogisticRatio.cpp solution:print.cpp \
	solution:OutputCVRP.cpp solution:ReadCVRP_Solution.cpp \
	solution:GetTransCostSingleRoute.cpp \
	CheckStockOut.cpp \
	Initial:SingleVehicle.cpp Initial.cpp Initial:Scheduling.cpp \
	InitialWithZone.cpp InitialScheduleWithCircleZone.cpp InitialScheduleWithBlockZone.cpp \
	LNS.cpp LNS:Destory.cpp LNS:Repair.cpp \
	Operator:PrintTempSolution.cpp Operator:GetTempLogisticRatio.cpp \
	Operator:SingleRoute.cpp Operator:Swap.cpp Operator:SwapWithBalancing.cpp \
	Operator:Repair.cpp \
	Operator:Insert.cpp Operator:Remove.cpp Operator:Balancing.cpp \
	-I $(andy_testpc) -I $(andy_testpc_boost) \
	-o irplr.out -O3
clean:
	rm -f *.txt *.csv *.o *.lp *.mps *.out
