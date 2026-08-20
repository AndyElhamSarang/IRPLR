#include "lib.h"
void exact::IRPMODEL(input &IRPLR, double &time_limit, double &memory_limit)
{

    IloEnv env;
    try
    {
        IloModel model(env);
        NumVarMatrix Inventory(env, IRPLR.TimeHorizon);
        NumVar4Matrix y(env, IRPLR.TimeHorizon);
        NumVar3Matrix q(env, IRPLR.TimeHorizon);
        NumVar3Matrix z(env, IRPLR.TimeHorizon);

        IloRangeArray c(env);
        IloIntArray obj(env);

        IloEnv env = model.getEnv();

        for (IloInt t = 0; t < IRPLR.TimeHorizon; t++)
        {
            Inventory[t] = IloNumVarArray(env, IRPLR.NumberOfRetailers + 1);
            for (IloInt i = 0; i < IRPLR.NumberOfRetailers + 1; i++)
            {
                Inventory[t][i] = IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            }
        }

        for (IloInt t = 0; t < IRPLR.TimeHorizon; t++)
        {
            y[t] = NumVar3Matrix(env, IRPLR.NumberOfVehicles);
            for (IloInt k = 0; k < IRPLR.NumberOfVehicles; k++)
            {
                y[t][k] = NumVarMatrix(env, IRPLR.NumberOfRetailers + 1);
                for (IloInt i = 0; i < IRPLR.NumberOfRetailers + 1; i++)
                {
                    y[t][k][i] = IloNumVarArray(env, IRPLR.NumberOfRetailers + 1);
                    for (IloInt j = 0; j < IRPLR.NumberOfRetailers + 1; j++)
                    {
                        if (j != i)
                        {
                            y[t][k][i][j] = IloNumVar(env, 0.0, 1.0, ILOINT);
                        }
                    }
                }
            }
        }

        for (IloInt t = 0; t < IRPLR.TimeHorizon; t++)
        {
            q[t] = NumVarMatrix(env, IRPLR.NumberOfVehicles);

            z[t] = NumVarMatrix(env, IRPLR.NumberOfVehicles);
            for (IloInt k = 0; k < IRPLR.NumberOfVehicles; k++)
            {
                q[t][k] = IloNumVarArray(env, IRPLR.NumberOfRetailers + 1);

                z[t][k] = IloNumVarArray(env, IRPLR.NumberOfRetailers + 1);
                for (IloInt i = 0; i < IRPLR.NumberOfRetailers; i++)
                {
                    q[t][k][i] = IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    z[t][k][i] = IloNumVar(env, 0.0, 1.0, ILOINT);
                }
            }
        }

        IloExpr LR(env);
        IloExpr exprTotalDelivery(env);
        IloExpr exprTransportation(env);

        for (IloInt t = 0; t < IRPLR.TimeHorizon; t++)
        {
            for (IloInt k = 0; k < IRPLR.NumberOfVehicles; k++)
            {
                for (IloInt i = 0; i < IRPLR.NumberOfRetailers; i++)
                {
                    exprTotalDelivery += q[t][k][i];
                }
            }
        }
        for (IloInt t = 0; t < IRPLR.TimeHorizon; t++)
        {
            for (IloInt k = 0; k < IRPLR.NumberOfVehicles; k++)
            {
                for (IloInt i = 0; i < IRPLR.NumberOfRetailers + 1; i++)
                {
                     for (IloInt j = 0; j < IRPLR.NumberOfRetailers + 1; j++)
                    {
                        if (j != i)
                        {
                        //    exprTransportation += IRPLR.Distance[i][j] * y[t][k][i][j];
                        exprTransportation +=  y[t][k][i][j];
                        }
                    }
                }
            }
        }

        LR = exprTransportation / exprTotalDelivery;
        model.add(IloMinimize(env, LR));

        model.add(c);
        IloCplex cplex(model);
        // cplex.setParam(IloCplex::TiLim, time_limit);
        // cplex.setParam(IloCplex::TreLim, memory_limit);
        // cplex.setParam(IloCplex::Threads, 1);
        // cplex.setParam(IloCplex::MemoryEmphasis, 1);
        //	cplex.setParam(IloCplex::MIPEmphasis,2);
        cplex.solve();
        env.out() << "Solution status = " << cplex.getStatus() << endl;
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;

        // cplex.exportModel("WSRP3index.lp");
    }
    catch (IloException &e)
    {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }

    env.end();
}
