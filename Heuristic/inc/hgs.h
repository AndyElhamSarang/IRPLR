class HGS
{
private:
public:
    // Call with: ./hgs instancePath solPath [-it nbIter] [-t myCPUtime] [-bks bksPath] [-seed mySeed] [-veh nbVehicles] [-log verbose]
    //[-it <int>] sets a maximum number of iterations without improvement. Defaults to 20,000
    //[-t <double>] sets a time limit in seconds. If this parameter is set, the code will be run iteratively until the time limit
    //[-seed <int>] sets a fixed seed. Defaults to 0
    //[-veh <int>] sets a prescribed fleet size. Otherwise a reasonable UB on the fleet size is calculated
    //[-round <bool>] rounding the distance to the nearest integer or not. It can be 0 (not rounding) or 1 (rounding). Defaults to 1.
    //[-log <bool>] sets the verbose level of the algorithm log. It can be 0 or 1. Defaults to 1.

    int HGSMaxNonimproveIt;
    double HGSTimelimit;
    int HGS_Seed;
    int DistanceRounding;
    int HGS_Printout;

    void ReadParameter();
    void CallHGS(input &IRPLR);
};
