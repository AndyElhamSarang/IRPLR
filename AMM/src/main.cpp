// Usage:
//   amm <instance.dat> [--algo ils|sa] [--obj lr|tc] [--time SECONDS] [--seed N]
//       [--vehicles K] [--csv results.csv] [--sol solution.txt]
//       [--maxIterRVND N] [--maxPerturb N] [--verbose]
#include "amm.h"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace
{
void writeSolution(const string &path, const Instance &in, const Solution &s)
{
    std::ofstream out(path);
    out << std::setprecision(10);
    out << "instance " << in.name << "\n";
    out << "routing_cost " << s.C << "\nquantity " << s.Qtot << "\nlogistic_ratio " << s.logisticRatio()
        << "\ntotal_cost " << s.C + s.holding() << "\n";
    for (int t = 0; t < in.H; ++t)
        for (int k = 0; k < in.K; ++k)
        {
            out << "period " << t + 1 << " vehicle " << k + 1 << " cost " << s.rc[t][k] << " load " << s.load[t][k]
                << " :";
            for (int i : s.R[t][k])
                out << " " << i + 1 << "(" << s.q[i][t] << ")";
            out << "\n";
        }
}
} // namespace

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: amm <instance.dat> [--algo ils|sa] [--obj lr|tc] [--time S] [--seed N] "
                     "[--vehicles K] [--csv FILE] [--sol FILE] [--maxIterRVND N] [--maxPerturb N] [--verbose]\n";
        return 1;
    }
    string instancePath = argv[1], csvPath, solPath;
    Params P;
    int vehicles = 0;
    for (int a = 2; a < argc; ++a)
    {
        string arg = argv[a];
        auto next = [&]() -> string {
            if (a + 1 >= argc)
            {
                std::cerr << "missing value for " << arg << "\n";
                std::exit(1);
            }
            return argv[++a];
        };
        if (arg == "--algo")
            P.algo = next();
        else if (arg == "--obj")
            P.obj = (next() == "tc") ? Objective::TC : Objective::LR;
        else if (arg == "--time")
            P.timeLimit = std::stod(next());
        else if (arg == "--seed")
            P.seed = static_cast<unsigned>(std::stoul(next()));
        else if (arg == "--vehicles")
            vehicles = std::stoi(next());
        else if (arg == "--csv")
            csvPath = next();
        else if (arg == "--sol")
            solPath = next();
        else if (arg == "--maxIterRVND")
            P.maxIterRVND = std::stoi(next());
        else if (arg == "--maxPerturb")
            P.maxPerturb = std::stoi(next());
        else if (arg == "--verbose")
            P.verbose = true;
        else
        {
            std::cerr << "unknown option " << arg << "\n";
            return 1;
        }
    }
    if (P.algo != "ils" && P.algo != "sa")
    {
        std::cerr << "--algo must be ils or sa\n";
        return 1;
    }

    Instance in;
    if (!in.read(instancePath, vehicles))
        return 1;
    Context cx(in, P);
    RunResult res = (P.algo == "ils") ? runILS(cx) : runSA(cx);
    double total = cx.elapsed();

    string verdict = "infeasible_construction";
    if (res.feasible)
    {
        string err = res.best.check();
        verdict = err.empty() ? "ok" : "CHECK_FAILED: " + err;
    }

    std::cout << std::setprecision(10);
    std::cout << in.name << " n=" << in.n << " H=" << in.H << " K=" << in.K << " algo=" << P.algo
              << " obj=" << (P.obj == Objective::LR ? "lr" : "tc") << " seed=" << P.seed;
    if (res.feasible)
        std::cout << " C=" << res.best.C << " Q=" << res.best.Qtot << " LR=" << res.best.logisticRatio()
                  << " TC=" << res.best.C + res.best.holding() << " time_to_best=" << res.timeToBest
                  << " total_time=" << total << " iterations=" << res.iterations;
    std::cout << " check=" << verdict << "\n";

    if (!csvPath.empty())
    {
        bool exists = std::ifstream(csvPath).good();
        std::ofstream csv(csvPath, std::ios::app);
        csv << std::setprecision(10);
        if (!exists)
            csv << "instance,n,H,K,algo,objective,seed,time_limit,routing_cost,quantity,logistic_ratio,total_cost,"
                   "time_to_best,total_time,iterations,status\n";
        csv << in.name << "," << in.n << "," << in.H << "," << in.K << "," << P.algo << ","
            << (P.obj == Objective::LR ? "lr" : "tc") << "," << P.seed << "," << P.timeLimit << ",";
        if (res.feasible)
            csv << res.best.C << "," << res.best.Qtot << "," << res.best.logisticRatio() << ","
                << res.best.C + res.best.holding() << "," << res.timeToBest << "," << total << "," << res.iterations;
        else
            csv << ",,,,,," << total;
        csv << "," << (verdict == "ok" ? "ok" : (res.feasible ? "check_failed" : "infeasible")) << "\n";
    }
    if (!solPath.empty() && res.feasible)
        writeSolution(solPath, in, res.best);
    return verdict == "ok" ? 0 : 2;
}
