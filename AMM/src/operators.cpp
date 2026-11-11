// Local search operators of AMM s3.2.1. Every operator explores its whole neighbourhood
// with the best-improvement strategy and applies the best improving move, if any.
// Only feasible solutions are accepted (AMM s3.2.1).
//
// Routing operators (Or-opt, Shift, Swap) move customers together with their current
// delivery quantities; quantities are never changed by these moves.
#include "amm.h"

namespace
{
inline int nodeAt(const vector<int> &r, int p)
{
    return (p < 0 || p >= static_cast<int>(r.size())) ? 0 : r[p] + 1;
}

double segmentLoad(const Solution &s, int t, const vector<int> &r, int p, int len)
{
    double l = 0;
    for (int x = p; x < p + len; ++x)
        l += s.q[r[x]][t];
    return l;
}
} // namespace

// ------------------------------------------------------------------ helpers

double routeCost(const Instance &in, const vector<int> &r)
{
    if (r.empty())
        return 0;
    double cost = in.c[0][r.front() + 1] + in.c[r.back() + 1][0];
    for (size_t x = 0; x + 1 < r.size(); ++x)
        cost += in.c[r[x] + 1][r[x + 1] + 1];
    return cost;
}

double removalDelta(const Instance &in, const vector<int> &r, int p, int len)
{
    int a = nodeAt(r, p - 1), b = nodeAt(r, p + len);
    int f = r[p] + 1, l = r[p + len - 1] + 1;
    return in.c[a][b] - in.c[a][f] - in.c[l][b];
}

double insertionDelta(const Instance &in, const vector<int> &r, int pos, int firstNode, int lastNode)
{
    int u = nodeAt(r, pos - 1), v = nodeAt(r, pos);
    return in.c[u][firstNode] + in.c[lastNode][v] - in.c[u][v];
}

double incLimit(const Solution &s, int i, int t)
{
    const Instance &in = *s.in;
    double lim = INF;
    for (int tt = t; tt < in.H; ++tt)
    {
        double before = (tt == 0) ? in.I0[i] : s.I[i][tt - 1];
        lim = std::min(lim, in.U[i] - before - s.q[i][tt]);
        lim = std::min(lim, s.S[tt]);
    }
    return std::max(0.0, lim);
}

double decLimit(const Solution &s, int i, int t)
{
    double lim = INF;
    for (int tt = t; tt < s.in->H; ++tt)
        lim = std::min(lim, s.I[i][tt]);
    return std::max(0.0, lim);
}

bool custFeasible(const Solution &s, int i, const vector<double> &nq, double &sumDI)
{
    const Instance &in = *s.in;
    double level = in.I0[i], cumulative = 0;
    sumDI = 0;
    for (int t = 0; t < in.H; ++t)
    {
        if (nq[t] < -EPS)
            return false;
        if (level + nq[t] > in.U[i] + EPS)
            return false;
        level += nq[t] - in.d[i];
        if (level < -EPS)
            return false;
        cumulative += nq[t] - s.q[i][t];
        if (s.S[t] - cumulative < -EPS)
            return false;
        sumDI += cumulative;
    }
    return true;
}

void removeVisit(Solution &s, int i, int t)
{
    int k = s.veh[i][t];
    int p = s.posOf(i, t);
    s.R[t][k].erase(s.R[t][k].begin() + p);
    s.veh[i][t] = -1;
    s.setQ(i, t, 0.0);
    s.refreshRoute(t, k);
    s.refreshCustomer(i);
    s.refreshSupplier();
}

void insertVisit(Solution &s, int i, int t, int k, int pos, double qty)
{
    s.R[t][k].insert(s.R[t][k].begin() + pos, i);
    s.setQ(i, t, qty);
    s.refreshRoute(t, k);
    s.refreshCustomer(i);
    s.refreshSupplier();
}

void changeQuantity(Solution &s, int i, int t, double qty)
{
    s.setQ(i, t, qty);
    s.refreshRoute(t, s.veh[i][t]);
    s.refreshCustomer(i);
    s.refreshSupplier();
}

std::pair<double, int> bestInsertion(const Solution &s, int t, int k, int i)
{
    const vector<int> &r = s.R[t][k];
    double best = INF;
    int bestPos = 0;
    for (int pos = 0; pos <= static_cast<int>(r.size()); ++pos)
    {
        double d = insertionDelta(*s.in, r, pos, i + 1, i + 1);
        if (d < best)
        {
            best = d;
            bestPos = pos;
        }
    }
    return {best, bestPos};
}

int firstEmptyVehicle(const Solution &s, int t)
{
    for (int k = 0; k < s.in->K; ++k)
        if (s.R[t][k].empty())
            return k;
    return -1;
}

// ------------------------------------------------------------------ Or-opt(k), k = 1,2,3
// Moves k adjacent customers to another position of the same route.
bool opOrOpt(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    double best = -cx.improveEps;
    int bt = -1, bk = -1, bp = -1, blen = -1, bpos = -1;
    for (int t = 0; t < in.H; ++t)
        for (int k = 0; k < in.K; ++k)
        {
            const vector<int> &r = s.R[t][k];
            int L = static_cast<int>(r.size());
            for (int len = 1; len <= 3 && len < L; ++len)
                for (int p = 0; p + len <= L; ++p)
                {
                    if (cx.timeUp())
                        return false;
                    double dR = removalDelta(in, r, p, len);
                    int f = r[p] + 1, l = r[p + len - 1] + 1;
                    int remaining = L - len;
                    auto remNode = [&](int j) {
                        if (j < 0 || j >= remaining)
                            return 0;
                        return r[j < p ? j : j + len] + 1;
                    };
                    for (int pos = 0; pos <= remaining; ++pos)
                    {
                        if (pos == p)
                            continue;
                        int u = remNode(pos - 1), v = remNode(pos);
                        double dC = dR + in.c[u][f] + in.c[l][v] - in.c[u][v];
                        double d = cx.dObj(s, dC, 0, 0);
                        if (d < best)
                        {
                            best = d;
                            bt = t, bk = k, bp = p, blen = len, bpos = pos;
                        }
                    }
                }
        }
    if (bt < 0)
        return false;
    vector<int> &r = s.R[bt][bk];
    vector<int> seg(r.begin() + bp, r.begin() + bp + blen);
    r.erase(r.begin() + bp, r.begin() + bp + blen);
    r.insert(r.begin() + bpos, seg.begin(), seg.end());
    s.refreshRoute(bt, bk);
    return true;
}

// ------------------------------------------------------------------ Shift(k), k = 1,2,3
// Moves k adjacent customers from one route to another route of the same period.
bool opShift(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    if (in.K < 2)
        return false;
    double best = -cx.improveEps;
    int bt = -1, bk1 = -1, bk2 = -1, bp = -1, blen = -1, bpos = -1;
    for (int t = 0; t < in.H; ++t)
    {
        int empty = firstEmptyVehicle(s, t);
        for (int k1 = 0; k1 < in.K; ++k1)
        {
            const vector<int> &r1 = s.R[t][k1];
            int L1 = static_cast<int>(r1.size());
            if (L1 == 0)
                continue;
            for (int k2 = 0; k2 < in.K; ++k2)
            {
                if (k2 == k1 || (s.R[t][k2].empty() && k2 != empty))
                    continue;
                const vector<int> &r2 = s.R[t][k2];
                int L2 = static_cast<int>(r2.size());
                double freeCap = in.Q - s.load[t][k2];
                for (int len = 1; len <= 3 && len <= L1; ++len)
                    for (int p = 0; p + len <= L1; ++p)
                    {
                        if (cx.timeUp())
                            return false;
                        if (segmentLoad(s, t, r1, p, len) > freeCap + EPS)
                            continue;
                        double dR = removalDelta(in, r1, p, len);
                        int f = r1[p] + 1, l = r1[p + len - 1] + 1;
                        for (int pos = 0; pos <= L2; ++pos)
                        {
                            double dC = dR + insertionDelta(in, r2, pos, f, l);
                            double d = cx.dObj(s, dC, 0, 0);
                            if (d < best)
                            {
                                best = d;
                                bt = t, bk1 = k1, bk2 = k2, bp = p, blen = len, bpos = pos;
                            }
                        }
                    }
            }
        }
    }
    if (bt < 0)
        return false;
    vector<int> &r1 = s.R[bt][bk1];
    vector<int> &r2 = s.R[bt][bk2];
    vector<int> seg(r1.begin() + bp, r1.begin() + bp + blen);
    r1.erase(r1.begin() + bp, r1.begin() + bp + blen);
    r2.insert(r2.begin() + bpos, seg.begin(), seg.end());
    s.refreshRoute(bt, bk1);
    s.refreshRoute(bt, bk2);
    return true;
}

// ------------------------------------------------------------------ Swap(k1,k2), k1 >= k2, k1,k2 in {1,2}
// Exchanges k1 adjacent customers of one route with k2 adjacent customers of another
// route of the same period.
bool opSwap(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    if (in.K < 2)
        return false;
    double best = -cx.improveEps;
    int bt = -1, bk1 = -1, bk2 = -1, bp1 = -1, bp2 = -1, bl1 = -1, bl2 = -1;
    for (int t = 0; t < in.H; ++t)
        for (int k1 = 0; k1 < in.K; ++k1)
            for (int k2 = 0; k2 < in.K; ++k2)
            {
                if (k1 == k2)
                    continue;
                const vector<int> &r1 = s.R[t][k1];
                const vector<int> &r2 = s.R[t][k2];
                int L1 = static_cast<int>(r1.size()), L2 = static_cast<int>(r2.size());
                if (L1 == 0 || L2 == 0)
                    continue;
                for (int len1 = 1; len1 <= 2 && len1 <= L1; ++len1)
                    for (int len2 = 1; len2 <= len1 && len2 <= L2; ++len2)
                        for (int p1 = 0; p1 + len1 <= L1; ++p1)
                        {
                            if (cx.timeUp())
                                return false;
                            double la = segmentLoad(s, t, r1, p1, len1);
                            int af = r1[p1] + 1, al = r1[p1 + len1 - 1] + 1;
                            int a1 = nodeAt(r1, p1 - 1), b1 = nodeAt(r1, p1 + len1);
                            for (int p2 = 0; p2 + len2 <= L2; ++p2)
                            {
                                double lb = segmentLoad(s, t, r2, p2, len2);
                                if (s.load[t][k1] - la + lb > in.Q + EPS ||
                                    s.load[t][k2] - lb + la > in.Q + EPS)
                                    continue;
                                int bf = r2[p2] + 1, bl = r2[p2 + len2 - 1] + 1;
                                int a2 = nodeAt(r2, p2 - 1), b2 = nodeAt(r2, p2 + len2);
                                double dC = in.c[a1][bf] + in.c[bl][b1] - in.c[a1][af] - in.c[al][b1] +
                                            in.c[a2][af] + in.c[al][b2] - in.c[a2][bf] - in.c[bl][b2];
                                double d = cx.dObj(s, dC, 0, 0);
                                if (d < best)
                                {
                                    best = d;
                                    bt = t, bk1 = k1, bk2 = k2, bp1 = p1, bp2 = p2, bl1 = len1, bl2 = len2;
                                }
                            }
                        }
            }
    if (bt < 0)
        return false;
    vector<int> &r1 = s.R[bt][bk1];
    vector<int> &r2 = s.R[bt][bk2];
    vector<int> segA(r1.begin() + bp1, r1.begin() + bp1 + bl1);
    vector<int> segB(r2.begin() + bp2, r2.begin() + bp2 + bl2);
    r1.erase(r1.begin() + bp1, r1.begin() + bp1 + bl1);
    r1.insert(r1.begin() + bp1, segB.begin(), segB.end());
    r2.erase(r2.begin() + bp2, r2.begin() + bp2 + bl2);
    r2.insert(r2.begin() + bp2, segA.begin(), segA.end());
    s.refreshRoute(bt, bk1);
    s.refreshRoute(bt, bk2);
    return true;
}

// ------------------------------------------------------------------ Increase/reduce deliveries
// OF1: increase as much as possible if h0 > hi, otherwise reduce as much as possible.
// OF2 (AMM s3.2.1, last paragraph): increase all deliveries; reduce only when the delivery
// can be removed completely (the visit is then dropped).
bool opIncreaseReduce(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    double best = -cx.improveEps;
    int bi = -1, bt = -1;
    double bq = 0;
    for (int i = 0; i < in.n; ++i)
    {
        if (cx.timeUp())
            return false;
        for (int t = 0; t < in.H; ++t)
        {
            int k = s.veh[i][t];
            if (k < 0)
                continue;
            double cur = s.q[i][t];
            double periods = in.H - t;
            bool tryIncrease = (cx.P.obj == Objective::LR) || (in.h0 > in.h[i]);
            bool tryReduce = (cx.P.obj == Objective::LR) || (in.h0 < in.h[i]);
            if (tryIncrease)
            {
                double inc = std::min(in.Q - s.load[t][k], incLimit(s, i, t));
                if (inc > EPS)
                {
                    double d = cx.dObj(s, 0, inc, (in.h[i] - in.h0) * inc * periods);
                    if (d < best)
                        best = d, bi = i, bt = t, bq = cur + inc;
                }
            }
            if (tryReduce)
            {
                double dec = std::min(cur, decLimit(s, i, t));
                if (cx.P.obj == Objective::LR && dec < cur - EPS)
                    dec = 0; // OF2: only complete removal
                if (dec > EPS)
                {
                    double dC = 0;
                    if (dec >= cur - EPS)
                    {
                        dec = cur;
                        dC = removalDelta(in, s.R[t][k], s.posOf(i, t), 1);
                    }
                    double d = cx.dObj(s, dC, -dec, (in.h[i] - in.h0) * (-dec) * periods);
                    if (d < best)
                        best = d, bi = i, bt = t, bq = cur - dec;
                }
            }
        }
    }
    if (bi < 0)
        return false;
    if (bq <= EPS)
        removeVisit(s, bi, bt);
    else
        changeQuantity(s, bi, bt, bq);
    return true;
}

// ------------------------------------------------------------------ Merge visits
// Merges two visits of a customer in periods t1 != t2 into a single visit in t1 or t2.
bool opMerge(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    double best = -cx.improveEps;
    int bi = -1, bkeep = -1, bdrop = -1;
    vector<double> nq(in.H);
    for (int i = 0; i < in.n; ++i)
    {
        if (cx.timeUp())
            return false;
        for (int t1 = 0; t1 < in.H; ++t1)
        {
            if (s.veh[i][t1] < 0)
                continue;
            for (int t2 = 0; t2 < in.H; ++t2)
            {
                if (t2 == t1 || s.veh[i][t2] < 0)
                    continue;
                // keep the visit in t1, drop the one in t2
                if (s.load[t1][s.veh[i][t1]] + s.q[i][t2] > in.Q + EPS)
                    continue;
                nq = s.q[i];
                nq[t1] += nq[t2];
                nq[t2] = 0;
                double sumDI;
                if (!custFeasible(s, i, nq, sumDI))
                    continue;
                double dC = removalDelta(in, s.R[t2][s.veh[i][t2]], s.posOf(i, t2), 1);
                double d = cx.dObj(s, dC, 0, (in.h[i] - in.h0) * sumDI);
                if (d < best)
                    best = d, bi = i, bkeep = t1, bdrop = t2;
            }
        }
    }
    if (bi < 0)
        return false;
    double merged = s.q[bi][bkeep] + s.q[bi][bdrop];
    removeVisit(s, bi, bdrop);
    changeQuantity(s, bi, bkeep, merged);
    return true;
}

// ------------------------------------------------------------------ Transfer visits
// Moves a visit (with its quantity) to a period in which the customer is not visited.
// The visit is inserted at the cheapest feasible position over the vehicles of that period.
bool opTransfer(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    double best = -cx.improveEps;
    int bi = -1, bfrom = -1, bto = -1, bk = -1, bpos = -1;
    vector<double> nq(in.H);
    for (int i = 0; i < in.n; ++i)
    {
        if (cx.timeUp())
            return false;
        for (int t = 0; t < in.H; ++t)
        {
            if (s.veh[i][t] < 0)
                continue;
            double qty = s.q[i][t];
            double dRem = removalDelta(in, s.R[t][s.veh[i][t]], s.posOf(i, t), 1);
            for (int t2 = 0; t2 < in.H; ++t2)
            {
                if (t2 == t || s.veh[i][t2] >= 0)
                    continue;
                nq = s.q[i];
                nq[t2] = qty;
                nq[t] = 0;
                double sumDI;
                if (!custFeasible(s, i, nq, sumDI))
                    continue;
                int empty = firstEmptyVehicle(s, t2);
                for (int k = 0; k < in.K; ++k)
                {
                    if ((s.R[t2][k].empty() && k != empty) || s.load[t2][k] + qty > in.Q + EPS)
                        continue;
                    auto [dIns, pos] = bestInsertion(s, t2, k, i);
                    double d = cx.dObj(s, dRem + dIns, 0, (in.h[i] - in.h0) * sumDI);
                    if (d < best)
                        best = d, bi = i, bfrom = t, bto = t2, bk = k, bpos = pos;
                }
            }
        }
    }
    if (bi < 0)
        return false;
    double qty = s.q[bi][bfrom];
    removeVisit(s, bi, bfrom);
    insertVisit(s, bi, bto, bk, bpos, qty);
    return true;
}

// ------------------------------------------------------------------ Insert visits
// Adds a new visit delivering the maximum possible quantity in a period in which the
// customer is not visited. OF1: only customers with h0 > hi; OF2: all customers.
bool opInsert(Context &cx, Solution &s)
{
    const Instance &in = cx.in;
    double best = -cx.improveEps;
    int bi = -1, bt = -1, bk = -1, bpos = -1;
    double bq = 0;
    for (int i = 0; i < in.n; ++i)
    {
        if (cx.timeUp())
            return false;
        if (cx.P.obj == Objective::TC && !(in.h0 > in.h[i]))
            continue;
        for (int t = 0; t < in.H; ++t)
        {
            if (s.veh[i][t] >= 0)
                continue;
            double lim = incLimit(s, i, t);
            if (lim <= EPS)
                continue;
            int empty = firstEmptyVehicle(s, t);
            for (int k = 0; k < in.K; ++k)
            {
                if (s.R[t][k].empty() && k != empty)
                    continue;
                double qty = std::min(lim, in.Q - s.load[t][k]);
                if (qty <= EPS)
                    continue;
                auto [dIns, pos] = bestInsertion(s, t, k, i);
                double d = cx.dObj(s, dIns, qty, (in.h[i] - in.h0) * qty * (in.H - t));
                if (d < best)
                    best = d, bi = i, bt = t, bk = k, bpos = pos, bq = qty;
            }
        }
    }
    if (bi < 0)
        return false;
    insertVisit(s, bi, bt, bk, bpos, bq);
    return true;
}

bool applyOperator(Context &cx, Solution &s, int op)
{
    switch (op)
    {
    case 0: return opOrOpt(cx, s);
    case 1: return opShift(cx, s);
    case 2: return opSwap(cx, s);
    case 3: return opIncreaseReduce(cx, s);
    case 4: return opMerge(cx, s);
    case 5: return opTransfer(cx, s);
    case 6: return opInsert(cx, s);
    }
    return false;
}
