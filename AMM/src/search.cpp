#include "amm.h"

#include <iostream>

Context::Context(const Instance &instance, const Params &params)
    : in(instance), P(params), rng(params.seed)
{
    start = std::chrono::steady_clock::now();
    deadline = start + std::chrono::milliseconds(static_cast<long long>(params.timeLimit * 1000.0));
    improveEps = (params.obj == Objective::LR) ? 1e-10 : 1e-6;
}

double Context::elapsed() const
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

int Context::randInt(int a, int b)
{
    return std::uniform_int_distribution<int>(a, b)(rng);
}

double Context::rand01()
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

double Context::dObj(const Solution &s, double dC, double dQ, double dH) const
{
    if (P.obj == Objective::LR)
    {
        double newQ = s.Qtot + dQ;
        if (newQ <= EPS)
            return INF;
        return (s.C + dC) / newQ - s.C / s.Qtot;
    }
    return dC + dH;
}

// AMM Algorithm 3: operators are drawn at random from the set V; an improving operator
// resets V, a non-improving one is removed from V.
bool rvnd(Context &cx, Solution &s)
{
    vector<int> all(NUM_OPERATORS);
    for (int o = 0; o < NUM_OPERATORS; ++o)
        all[o] = o;
    vector<int> V = all;
    bool improved = false;
    while (!V.empty() && !cx.timeUp())
    {
        int idx = cx.randInt(0, static_cast<int>(V.size()) - 1);
        if (applyOperator(cx, s, V[idx]))
        {
            V = all;
            improved = true;
        }
        else
            V.erase(V.begin() + idx);
    }
    return improved;
}

// Multi-start RVND (AMM s3.2.1): RVND is applied maxIterRVND times to the same starting
// solution (different random operator orders); the best result is returned.
Solution multiRVND(Context &cx, const Solution &s0)
{
    Solution best = s0;
    double bestF = cx.f(s0);
    for (int it = 0; it < cx.P.maxIterRVND && !cx.timeUp(); ++it)
    {
        Solution s = s0;
        rvnd(cx, s);
        double f = cx.f(s);
        if (f < bestF - cx.improveEps)
        {
            best = s;
            bestF = f;
        }
    }
    return best;
}

RunResult runILS(Context &cx)
{
    RunResult res;
    Solution s0;
    if (!construct(cx, s0))
        return res;
    res.feasible = true;
    res.best = multiRVND(cx, s0);
    res.timeToBest = cx.elapsed();
    double bestF = cx.f(res.best);
    if (cx.P.verbose)
        std::cerr << "[ILS] initial " << cx.f(s0) << " after LS " << bestF << "\n";
    while (!cx.timeUp())
    {
        Solution s = res.best;
        perturb(cx, s);
        s = multiRVND(cx, s);
        ++res.iterations;
        double f = cx.f(s);
        if (f < bestF - cx.improveEps)
        {
            res.best = s;
            bestF = f;
            res.timeToBest = cx.elapsed();
            if (cx.P.verbose)
                std::cerr << "[ILS] it " << res.iterations << " t=" << cx.elapsed() << " f=" << bestF << "\n";
        }
    }
    return res;
}

RunResult runSA(Context &cx)
{
    RunResult res;
    Solution s0;
    if (!construct(cx, s0))
        return res;
    res.feasible = true;
    res.best = s0;
    res.timeToBest = cx.elapsed();
    double bestF = cx.f(s0);
    vector<Solution> cur(cx.P.nSol, s0);
    vector<double> curF(cx.P.nSol, bestF);
    double temp = cx.P.temp0;
    while (!cx.timeUp() && temp > cx.P.tempMin)
    {
        for (int it = 0; it < cx.P.maxI && !cx.timeUp(); ++it)
            for (int x = 0; x < cx.P.nSol; ++x)
            {
                Solution nb = cur[x];
                if (!randomMove(cx, nb, cx.randInt(0, NUM_OPERATORS - 1)))
                    continue;
                double f = cx.f(nb);
                double delta = f - curF[x];
                if (delta < 0 || cx.rand01() < std::exp(-delta / temp))
                {
                    cur[x] = std::move(nb);
                    curF[x] = f;
                }
            }
        for (int x = 0; x < cx.P.nSol && !cx.timeUp(); ++x)
        {
            cur[x] = multiRVND(cx, cur[x]);
            curF[x] = cx.f(cur[x]);
            if (curF[x] < bestF - cx.improveEps)
            {
                res.best = cur[x];
                bestF = curF[x];
                res.timeToBest = cx.elapsed();
                if (cx.P.verbose)
                    std::cerr << "[SA] T=" << temp << " t=" << cx.elapsed() << " f=" << bestF << "\n";
            }
        }
        for (int x = 0; x < cx.P.nSol; ++x)
        {
            cur[x] = res.best;
            curF[x] = bestF;
        }
        temp *= cx.P.alpha;
        ++res.iterations;
    }
    return res;
}
