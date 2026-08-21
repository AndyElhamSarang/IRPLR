extern int printout_inputdata;
extern int printout_initialSchedule;
extern int printout_initialOutputCVRP;
extern int printout_initialRouting;
extern int printout_initialReadCVRP;
extern int printout_initial;
extern int Global_total_iteration;
extern base_generator_type generator;
extern ofstream Table;
extern string MachineDirectory;
extern int OutputResults;
extern int NumberOfInitialSolutions;
extern int NumberOfExperiments;
extern string AllowLagrangianRelaxation;
extern string TypeOfRebalance;
extern double InitialLagrangianScalar;
extern double ToAdjustLagrangianScalar;
extern int ToTriggerAdjustment;
extern int GridResolution;
extern double power;
extern double MainAlgorithmTimeLimit;
extern double AccumulatedTimeHGS;


extern int NumberOfBalacingOperatorCalled;
extern int NumberOfVehicleAtMinimumDelivery;

extern bool whether_results_reported_30;
extern bool whether_results_reported_60;
extern bool whether_results_reported_first_improvement;
extern string OutputSolutionJSON;
extern string ActivateHGS;



extern time_t start_time; //General use to count time from one point to another.
extern time_t end_time;
extern time_t start_time_to_best;
extern time_t end_time_to_best;
extern time_t total_start_time;
extern time_t total_end_time;
extern time_t HGS_start_time;
extern time_t HGS_end_time;

extern int theta;
extern int sigma; 
extern int phi;