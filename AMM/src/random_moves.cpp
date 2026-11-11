// Random moves: SA neighbours (AMM s3.3.1) and the ILS perturbation (AMM s3.2.2).
// All moves keep the solution feasible (AMM: "only feasible solutions are allowed").
#include "amm.h"

namespace
{
constexpr int ATTEMPTS = 30; // tries to draw a feasible random move before giving up

vector<std::pair<int, int>> visits(const Solution &s)
{
    vector<std::pair<int, int>> v;
    for (int i = 0; i < s.in->n; ++i)
        for (int t = 0; t < s.in->H; ++t)
            if (s.veh[i][t] >= 0)
                v.push_back({i, t});
    return v;
}

double segLoad(const Solution &s, int t, const vector<int> &r, int p, int len)
{
    double l = 0;
    for (int x = p; x < p + len; ++x)
        l += s.q[r[x]][t];
    return l;
}

// Cheapest feasible insertion of customer i with quantity qty in period t over all vehicles.
bool cheapestVehicleInsertion(const Solution &s, int i, int t, double qty, int &bk, int &bpos)
{
    double best = INF;
    bk = -1;
    for (int k = 0; k < s.in->K; ++k)
    {
        if (s.load[t][k] + qty > s.in->Q + EPS)
            continue;
        auto [d, pos] = bestInsertion(s, t, k, i);
        if (d < best)
            best = d, bk = k, bpos = pos;
    }
    return bk >= 0;
}

// ---- SA neighbours, one per local search operator

bool randOrOpt(Context &cx, Solution &s)
{
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int t = cx.randInt(0, cx.in.H - 1), k = cx.randInt(0, cx.in.K - 1);
        vector<int> &r = s.R[t][k];
        int L = static_cast<int>(r.size());
        if (L < 2)
            continue;
        int len = cx.randInt(1, std::min(3, L - 1));
        int p = cx.randInt(0, L - len);
        int pos = cx.randInt(0, L - len);
        if (pos == p)
            continue;
        vector<int> seg(r.begin() + p, r.begin() + p + len);
        r.erase(r.begin() + p, r.begin() + p + len);
        r.insert(r.begin() + pos, seg.begin(), seg.end());
        s.refreshRoute(t, k);
        return true;
    }
    return false;
}

bool randShift(Context &cx, Solution &s)
{
    if (cx.in.K < 2)
        return false;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int t = cx.randInt(0, cx.in.H - 1);
        int k1 = cx.randInt(0, cx.in.K - 1), k2 = cx.randInt(0, cx.in.K - 1);
        if (k1 == k2 || s.R[t][k1].empty())
            continue;
        vector<int> &r1 = s.R[t][k1];
        vector<int> &r2 = s.R[t][k2];
        int L1 = static_cast<int>(r1.size());
        int len = cx.randInt(1, std::min(3, L1));
        int p = cx.randInt(0, L1 - len);
        if (s.load[t][k2] + segLoad(s, t, r1, p, len) > cx.in.Q + EPS)
            continue;
        int pos = cx.randInt(0, static_cast<int>(r2.size()));
        vector<int> seg(r1.begin() + p, r1.begin() + p + len);
        r1.erase(r1.begin() + p, r1.begin() + p + len);
        r2.insert(r2.begin() + pos, seg.begin(), seg.end());
        s.refreshRoute(t, k1);
        s.refreshRoute(t, k2);
        return true;
    }
    return false;
}

bool randSwap(Context &cx, Solution &s)
{
    if (cx.in.K < 2)
        return false;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int t = cx.randInt(0, cx.in.H - 1);
        int k1 = cx.randInt(0, cx.in.K - 1), k2 = cx.randInt(0, cx.in.K - 1);
        if (k1 == k2 || s.R[t][k1].empty() || s.R[t][k2].empty())
            continue;
        vector<int> &r1 = s.R[t][k1];
        vector<int> &r2 = s.R[t][k2];
        int L1 = static_cast<int>(r1.size()), L2 = static_cast<int>(r2.size());
        int len1 = cx.randInt(1, std::min(2, L1));
        int len2 = cx.randInt(1, std::min(len1, L2));
        int p1 = cx.randInt(0, L1 - len1), p2 = cx.randInt(0, L2 - len2);
        double la = segLoad(s, t, r1, p1, len1), lb = segLoad(s, t, r2, p2, len2);
        if (s.load[t][k1] - la + lb > cx.in.Q + EPS || s.load[t][k2] - lb + la > cx.in.Q + EPS)
            continue;
        vector<int> segA(r1.begin() + p1, r1.begin() + p1 + len1);
        vector<int> segB(r2.begin() + p2, r2.begin() + p2 + len2);
        r1.erase(r1.begin() + p1, r1.begin() + p1 + len1);
        r1.insert(r1.begin() + p1, segB.begin(), segB.end());
        r2.erase(r2.begin() + p2, r2.begin() + p2 + len2);
        r2.insert(r2.begin() + p2, segA.begin(), segA.end());
        s.refreshRoute(t, k1);
        s.refreshRoute(t, k2);
        return true;
    }
    return false;
}

bool randIncreaseReduce(Context &cx, Solution &s)
{
    auto v = visits(s);
    if (v.empty())
        return false;
    const Instance &in = cx.in;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        auto [i, t] = v[cx.randInt(0, static_cast<int>(v.size()) - 1)];
        int k = s.veh[i][t];
        double cur = s.q[i][t];
        bool increase;
        if (cx.P.obj == Objective::LR)
            increase = cx.rand01() < 0.5;
        else
            increase = in.h0 > in.h[i];
        if (increase)
        {
            double inc = std::min(in.Q - s.load[t][k], incLimit(s, i, t));
            if (inc <= EPS)
                continue;
            changeQuantity(s, i, t, cur + inc);
            return true;
        }
        double dec = std::min(cur, decLimit(s, i, t));
        if (cx.P.obj == Objective::LR && dec < cur - EPS)
            continue; // OF2: only complete removal
        if (dec <= EPS)
            continue;
        if (dec >= cur - EPS)
            removeVisit(s, i, t);
        else
            changeQuantity(s, i, t, cur - dec);
        return true;
    }
    return false;
}

bool randMerge(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    vector<int> candidates;
    for (int i = 0; i < in.n; ++i)
    {
        int count = 0;
        for (int t = 0; t < in.H; ++t)
            count += s.veh[i][t] >= 0;
        if (count >= 2)
            candidates.push_back(i);
    }
    if (candidates.empty())
        return false;
    vector<double> nq(in.H);
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int i = candidates[cx.randInt(0, static_cast<int>(candidates.size()) - 1)];
        vector<int> periods;
        for (int t = 0; t < in.H; ++t)
            if (s.veh[i][t] >= 0)
                periods.push_back(t);
        int keep = periods[cx.randInt(0, static_cast<int>(periods.size()) - 1)];
        int drop = periods[cx.randInt(0, static_cast<int>(periods.size()) - 1)];
        if (keep == drop || s.load[keep][s.veh[i][keep]] + s.q[i][drop] > in.Q + EPS)
            continue;
        nq = s.q[i];
        nq[keep] += nq[drop];
        nq[drop] = 0;
        double sumDI;
        if (!custFeasible(s, i, nq, sumDI))
            continue;
        double merged = nq[keep];
        removeVisit(s, i, drop);
        changeQuantity(s, i, keep, merged);
        return true;
    }
    return false;
}

bool randTransfer(Context &cx, Solution &s)
{
    auto v = visits(s);
    if (v.empty())
        return false;
    const Instance &in = cx.in;
    vector<double> nq(in.H);
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        auto [i, t] = v[cx.randInt(0, static_cast<int>(v.size()) - 1)];
        int t2 = cx.randInt(0, in.H - 1);
        if (t2 == t || s.veh[i][t2] >= 0)
            continue;
        double qty = s.q[i][t];
        nq = s.q[i];
        nq[t2] = qty;
        nq[t] = 0;
        double sumDI;
        if (!custFeasible(s, i, nq, sumDI))
            continue;
        int k, pos;
        if (!cheapestVehicleInsertion(s, i, t2, qty, k, pos))
            continue;
        removeVisit(s, i, t);
        insertVisit(s, i, t2, k, pos, qty);
        return true;
    }
    return false;
}

bool randInsert(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int i = cx.randInt(0, in.n - 1), t = cx.randInt(0, in.H - 1);
        if (s.veh[i][t] >= 0)
            continue;
        if (cx.P.obj == Objective::TC && !(in.h0 > in.h[i]))
            continue;
        int k = cx.randInt(0, in.K - 1);
        double qty = std::min(incLimit(s, i, t), in.Q - s.load[t][k]);
        if (qty <= EPS)
            continue;
        auto [d, pos] = bestInsertion(s, t, k, i);
        insertVisit(s, i, t, k, pos, qty);
        return true;
    }
    return false;
}

// ---- ILS perturbation operators (AMM s3.2.2). Each perturbs one element.

// Random shift: a random customer of a random route moves to another route of the same
// period (cheapest position).
bool pertShift(Context &cx, Solution &s)
{
    if (cx.in.K < 2)
        return false;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int t = cx.randInt(0, cx.in.H - 1);
        int k1 = cx.randInt(0, cx.in.K - 1), k2 = cx.randInt(0, cx.in.K - 1);
        if (k1 == k2 || s.R[t][k1].empty())
            continue;
        int p = cx.randInt(0, static_cast<int>(s.R[t][k1].size()) - 1);
        int i = s.R[t][k1][p];
        double qty = s.q[i][t];
        if (s.load[t][k2] + qty > cx.in.Q + EPS)
            continue;
        removeVisit(s, i, t);
        auto [d, pos] = bestInsertion(s, t, k2, i);
        insertVisit(s, i, t, k2, pos, qty);
        return true;
    }
    return false;
}

// Random delivery reduction: a random visit whose delivery can be feasibly reduced is cut
// as much as possible (the visit is dropped if its quantity reaches zero).
bool pertReduction(Context &cx, Solution &s)
{
    auto v = visits(s);
    if (v.empty())
        return false;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        auto [i, t] = v[cx.randInt(0, static_cast<int>(v.size()) - 1)];
        double cur = s.q[i][t];
        double dec = std::min(cur, decLimit(s, i, t));
        if (dec <= EPS)
            continue;
        if (dec >= cur - EPS)
            removeVisit(s, i, t);
        else
            changeQuantity(s, i, t, cur - dec);
        return true;
    }
    return false;
}

// Random insertion: a random route and a customer not served in its period; the largest
// feasible delivery is inserted at the cheapest position of that route.
bool pertInsertion(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        int t = cx.randInt(0, in.H - 1), k = cx.randInt(0, in.K - 1);
        int i = cx.randInt(0, in.n - 1);
        if (s.veh[i][t] >= 0)
            continue;
        double qty = std::min(incLimit(s, i, t), in.Q - s.load[t][k]);
        if (qty <= EPS)
            continue;
        auto [d, pos] = bestInsertion(s, t, k, i);
        insertVisit(s, i, t, k, pos, qty);
        return true;
    }
    return false;
}

// Random split: half of a random delivery is transferred to another random period
// (added to the existing visit there, or delivered by a new visit at the cheapest
// feasible position).
bool pertSplit(Context &cx, Solution &s)
{
    auto v = visits(s);
    if (v.empty() || cx.in.H < 2)
        return false;
    const Instance &in = cx.in;
    vector<double> nq(in.H);
    for (int a = 0; a < ATTEMPTS; ++a)
    {
        auto [i, t] = v[cx.randInt(0, static_cast<int>(v.size()) - 1)];
        int t2 = cx.randInt(0, in.H - 1);
        if (t2 == t)
            continue;
        double half = s.q[i][t] / 2.0;
        if (half <= EPS)
            continue;
        nq = s.q[i];
        nq[t] -= half;
        nq[t2] += half;
        double sumDI;
        if (!custFeasible(s, i, nq, sumDI))
            continue;
        if (s.veh[i][t2] >= 0)
        {
            if (s.load[t2][s.veh[i][t2]] + half > in.Q + EPS)
                continue;
            changeQuantity(s, i, t, nq[t]);
            changeQuantity(s, i, t2, nq[t2]);
            return true;
        }
        int k, pos;
        if (!cheapestVehicleInsertion(s, i, t2, half, k, pos))
            continue;
        changeQuantity(s, i, t, nq[t]);
        insertVisit(s, i, t2, k, pos, half);
        return true;
    }
    return false;
}
} // namespace

bool randomMove(Context &cx, Solution &s, int op)
{
    switch (op)
    {
    case 0: return randOrOpt(cx, s);
    case 1: return randShift(cx, s);
    case 2: return randSwap(cx, s);
    case 3: return randIncreaseReduce(cx, s);
    case 4: return randMerge(cx, s);
    case 5: return randTransfer(cx, s);
    case 6: return randInsert(cx, s);
    }
    return false;
}

// AMM s3.2.2: each call activates a single operator (chosen at random), which perturbs up
// to max_perturb elements; operators are drawn at random until one changes the solution.
// The number of elements is drawn uniformly from {1, ..., max_perturb}.
void perturb(Context &cx, Solution &s)
{
    using Op = bool (*)(Context &, Solution &);
    vector<Op> ops = {pertShift, pertReduction, pertInsertion, pertSplit};
    std::shuffle(ops.begin(), ops.end(), cx.rng);
    for (Op op : ops)
    {
        int changed = 0;
        int elements = cx.randInt(1, cx.P.maxPerturb);
        for (int e = 0; e < elements; ++e)
            changed += op(cx, s) ? 1 : 0;
        if (changed > 0)
            return;
    }
}
