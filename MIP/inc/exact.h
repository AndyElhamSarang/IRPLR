class exact
{
 private:

  
 public:

  
  void IRPMODEL(input &IRPLR,double &time_limit, double &memory_limit);

  typedef IloArray< IloNumVarArray > NumVarMatrix;
  typedef IloArray< NumVarMatrix >   NumVar3Matrix;
  typedef IloArray<NumVar3Matrix> NumVar4Matrix;
};
