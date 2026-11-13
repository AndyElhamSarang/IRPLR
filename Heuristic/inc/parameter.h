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
extern string InitialSolutionMethod;
extern string AllowLagrangianRelaxation;
extern string TypeOfRebalance;
extern double InitialLagrangianScalar;
extern double ToAdjustLagrangianScalar;
extern int ToTriggerAdjustment;
extern double power;
extern int DisturbStrength;
extern double MainAlgorithmTimeLimit;
extern double AccumulatedTimeHGS;
extern string Activate_Disturbance;
extern string LocalSearchOrder; // SEQUENTIAL (O1..O5 in fixed order) or RVND (random order, Alvarez et al. 2018)
extern bool UseVehicleSlack;   // YES: O1-O4 may borrow slack from a full vehicle for an inserted visit; NO: such moves fail


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

extern double theta;
extern int sigma; 
extern int phi;
extern int min_grid_resolution;
extern int grid_resolution_increment;