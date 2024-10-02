class solution_construction
{

private:
public:
void ReadParameter();

    void INITIAL_Single_Vehicle(input &IRPLR, solution &IRPSolution);
    void INITIAL(input &IRPLR, solution &IRPSolution, HGS & Routing);
    void Initial_Schedule(input &IRPLR, solution &IRPSolution);

    void INITIAL_ZONE(input &IRPLR, solution &IRPSolution, HGS &Routing);
    void Initial_CircleZone_Schedule(input &IRPLR, solution &IRPSolution);
    void Initial_BlockZone_Schedule(input &IRPLR, solution &IRPSolution);
};