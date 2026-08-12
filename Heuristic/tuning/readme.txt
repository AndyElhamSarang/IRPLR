

install.packages("fs", repos = "https://cloud.r-project.org")

install.packages("irace", repos = "https://cloud.r-project.org")

/home/andy/R/x86_64-pc-linux-gnu-library/4.3/irace/bin/irace --init


//To confirm installation
library(irace)
packageVersion("irace")
[1] ‘4.4.3’
system.file("bin", package = "irace")
[1] "/home/andy/R/x86_64-pc-linux-gnu-library/4.3/irace/bin"
Sys.which("irace")
irace 
   "" 
//To execute the program, use the following command in the terminal:
./IRPLR.out \
--instance instance.txt \
--NumberOfInitialSolutions 5 \
--NumberOfExperiments 1 \
--AllowLagrangianRelaxation YES \
--TypeOfRebalance NOT_GUARANTEE_FEASIBILITY \
--InitialLagrangianScalar 10 \
--ToAdjustLagrangianScalar 0.5 \
--ToTriggerAdjustment 20 \
--GridResolution 4 \
--ActivateHGS NO \
--theta 3 \
--sigma 10 \
--phi 10 \
--MainAlgorithmTimeLimit 65
