// Reimplementation of the ILS and SA of
//   Alvarez, A., Munari, P., Morabito, R. (2018). Iterated local search and simulated
//   annealing algorithms for the inventory routing problem. ITOR 25(6), 1785-1809.
// Section and line references in comments ("AMM s3.2.1") point to that paper.
// Interpretation choices where the paper is silent are listed in ../README.md.
#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <vector>

using std::string;
using std::vector;

constexpr double EPS = 1e-7;
constexpr double INF = std::numeric_limits<double>::infinity();

enum class Objective
{
    LR, // OF2: routing cost / total quantity delivered
    TC  // OF1: routing cost + inventory holding cost
};

struct Instance
{
    string name;
    int n = 0;  // number of customers (customer i is node i+1; node 0 is the depot)
    int H = 0;  // number of periods
    int K = 1;  // number of vehicles
    double Q = 0;   // vehicle capacity
    double I00 = 0; // initial supplier inventory
    double r = 0;   // supplier production per period
    double h0 = 0;  // supplier holding cost
    vector<double> I0, U, d, h; // per customer: initial inventory, capacity, demand, holding cost
    vector<vector<double>> c;   // NINT Euclidean distances, (n+1) x (n+1)

    // Reads an Archetti et al. (2007) format file. The number of vehicles is taken from the
    // file name suffix (abs1n10_3.dat -> 3) unless vehiclesOverride > 0.
    bool read(const string &path, int vehiclesOverride);
};

struct Params
{
    Objective obj = Objective::LR;
    string algo = "ils"; // "ils" or "sa"
    double timeLimit = 30.0;
    unsigned seed = 1;
    int maxIterRVND = 5; // AMM s4.2
    int maxPerturb = 7;  // AMM Table 1 (ILS)
    // SA, AMM Table 1
    int nSol = 10;
    int maxI = 25;
    double temp0 = 3000.0;
    double tempMin = 1e-6;
    double alpha = 0.99;
    bool verbose = false;
};

// Inventory convention (identical to the MSILS code in ../Heuristic):
//   customer: I[i][t] = I[i][t-1] + q[i][t] - d[i] >= 0 and I[i][t-1] + q[i][t] <= U[i]
//   supplier: S[t] = S[t-1] + r - sum_i q[i][t] >= 0
struct Solution
{
    const Instance *in = nullptr;
    vector<vector<vector<int>>> R; // R[t][k]: customers in visiting order
    vector<vector<double>> q;      // q[i][t]
    vector<vector<int>> veh;       // veh[i][t]: vehicle visiting i in t, -1 if none
    vector<vector<double>> load;   // load[t][k]
    vector<vector<double>> rc;     // rc[t][k]: route cost
    vector<vector<double>> I;      // I[i][t]: end-of-period customer inventory
    vector<double> S;              // S[t]: end-of-period supplier inventory
    vector<double> del;            // del[t]: total delivered in period t
    double C = 0;                  // total routing cost
    double Qtot = 0;               // total delivered quantity

    void init(const Instance &instance);
    void refreshRoute(int t, int k); // recompute rc, load and veh of one route; updates C
    void refreshCustomer(int i);     // recompute I[i][.]
    void refreshSupplier();          // recompute S from del
    void refreshAll();               // recompute everything from R and q
    void setQ(int i, int t, double value); // updates q, del and Qtot only

    int posOf(int i, int t) const;
    double holding() const;
    double objective(Objective o) const;
    double logisticRatio() const { return Qtot > EPS ? C / Qtot : INF; }
    // Independent from-scratch verification of every constraint. Returns "" if feasible.
    string check() const;
};

struct Context
{
    const Instance &in;
    Params P;
    std::mt19937 rng;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point deadline;
    double improveEps;

    Context(const Instance &instance, const Params &params);
    bool timeUp() const { return std::chrono::steady_clock::now() >= deadline; }
    double elapsed() const;
    int randInt(int a, int b); // uniform in [a, b]
    double rand01();
    double f(const Solution &s) const { return s.objective(P.obj); }
    // Objective change of a move with routing-cost change dC, quantity change dQ and
    // holding-cost change dH.
    double dObj(const Solution &s, double dC, double dQ, double dH) const;
};

// ---------- helpers (operators.cpp) ----------
double routeCost(const Instance &in, const vector<int> &r);
double removalDelta(const Instance &in, const vector<int> &r, int p, int len);
double insertionDelta(const Instance &in, const vector<int> &r, int pos, int firstNode, int lastNode);
// Largest feasible increase of q[i][t] w.r.t. customer capacity and supplier stock
// (vehicle capacity is checked separately).
double incLimit(const Solution &s, int i, int t);
// Largest feasible decrease of q[i][t] without stockout.
double decLimit(const Solution &s, int i, int t);
// Inventory/supplier feasibility of replacing q[i][.] by nq; sumDI = sum_t (new I - old I).
bool custFeasible(const Solution &s, int i, const vector<double> &nq, double &sumDI);
void removeVisit(Solution &s, int i, int t);
void insertVisit(Solution &s, int i, int t, int k, int pos, double qty);
void changeQuantity(Solution &s, int i, int t, double qty);
// Cheapest insertion position of customer i in route (t,k): returns {delta cost, position}.
std::pair<double, int> bestInsertion(const Solution &s, int t, int k, int i);
// Index of the first empty vehicle in period t (-1 if none); used to skip symmetric empty routes.
int firstEmptyVehicle(const Solution &s, int t);

// ---------- local search operators, best improvement (operators.cpp) ----------
// Each returns true if it found and applied an improving move.
bool opOrOpt(Context &cx, Solution &s);
bool opShift(Context &cx, Solution &s);
bool opSwap(Context &cx, Solution &s);
bool opIncreaseReduce(Context &cx, Solution &s);
bool opMerge(Context &cx, Solution &s);
bool opTransfer(Context &cx, Solution &s);
bool opInsert(Context &cx, Solution &s);
constexpr int NUM_OPERATORS = 7;
bool applyOperator(Context &cx, Solution &s, int op);

// ---------- random moves (random_moves.cpp) ----------
// SA neighbour: one random feasible move of operator op. Returns true if applied.
bool randomMove(Context &cx, Solution &s, int op);
// ILS perturbation (AMM s3.2.2).
void perturb(Context &cx, Solution &s);

// ---------- construction and metaheuristics ----------
bool construct(Context &cx, Solution &best); // AMM s3.1, Algorithm 1
bool rvnd(Context &cx, Solution &s);         // AMM Algorithm 3
Solution multiRVND(Context &cx, const Solution &s0);

struct RunResult
{
    bool feasible = false;
    Solution best;
    double timeToBest = 0;
    long long iterations = 0;
};
RunResult runILS(Context &cx); // AMM Algorithm 2
RunResult runSA(Context &cx);  // AMM Algorithm 4
