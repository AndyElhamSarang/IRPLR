CC=g++
andy = /home/andy/Desktop/Curtin/IRPLR/IRPLR/MIP/inc
andy_boost = /home/andy/Desktop/c++boost/boost_1_83_0
andy_desktop = /home/andy/Desktop/Curtin/IRPLR/IRPLR/MIP/inc
andy_desktop_boost = /home/andy/Desktop/c++boost/boost_1_83_0
andy_lenovo = /home/andy/Desktop/Curtin/IRPLR/IRPLR/MIP/inc
andy_lenovo_boost = /home/andy/Desktop/c++_boost/boost_1_83_0
andy_testpc = /home/andytestpc/Desktop/Curtin/IRPLR/IRPLR/MIP/inc
andy_testpc_boost = /home/andytestpc/Desktop/c++_boost/boost_1_83_0

# Set CPLEX_HOME to the CPLEX Studio installation directory before using this target.
CPLEX_HOME ?= /opt/ibm/ILOG/CPLEX_Studio2211
CPLEX_INC = -I$(CPLEX_HOME)/cplex/include -I$(CPLEX_HOME)/concert/include
CPLEX_LIB = -L$(CPLEX_HOME)/cplex/lib/x86-64_linux/static_pic \
	-L$(CPLEX_HOME)/concert/lib/x86-64_linux/static_pic
CPLEX_LIBS = -lilocplex -lcplex -lconcert -lpthread -ldl
andy:
	$(CC) main.cpp file:Read_andy.cpp file:ReadInstanceName.cpp file:GlobalParameter.cpp \
	Input:Read.cpp \
	PrintData.cpp \
	IRPMODEL.cpp \
	-I $(andy) -I $(andy_boost) \
	$(CPLEX_INC) \
	-DIL_STD -O3 $(CPLEX_LIB) $(CPLEX_LIBS) \
	-o MS.out -O3
clean:
	rm -f *.txt *.csv *.o *.lp *.mps *.out

cplex:
	$(CC) main_cplex.cpp IRPLRModel.cpp file:Read_andy.cpp file:ReadInstanceName.cpp \
	file:GlobalParameter.cpp Input:Read.cpp PrintData.cpp $(CPLEX_INC) \
	-I ../inc -I $(andy) -I /usr/include \
	-DIL_STD -o MIP_CPLEX.out -O2 $(CPLEX_LIB) $(CPLEX_LIBS)
