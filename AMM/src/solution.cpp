#include "amm.h"

#include <sstream>

void Solution::init(const Instance &instance)
{
    in = &instance;
    R.assign(in->H, vector<vector<int>>(in->K));
    q.assign(in->n, vector<double>(in->H, 0.0));
    veh.assign(in->n, vector<int>(in->H, -1));
    load.assign(in->H, vector<double>(in->K, 0.0));
    rc.assign(in->H, vector<double>(in->K, 0.0));
    I.assign(in->n, vector<double>(in->H, 0.0));
    S.assign(in->H, 0.0);
    del.assign(in->H, 0.0);
    refreshAll();
}

void Solution::refreshRoute(int t, int k)
{
    const vector<int> &r = R[t][k];
    double cost = routeCost(*in, r);
    C += cost - rc[t][k];
    rc[t][k] = cost;
    double l = 0;
    for (int i : r)
    {
        l += q[i][t];
        veh[i][t] = k;
    }
    load[t][k] = l;
}

void Solution::refreshCustomer(int i)
{
    double level = in->I0[i];
    for (int t = 0; t < in->H; ++t)
    {
        level += q[i][t] - in->d[i];
        I[i][t] = level;
    }
}

void Solution::refreshSupplier()
{
    double level = in->I00;
    for (int t = 0; t < in->H; ++t)
    {
        level += in->r - del[t];
        S[t] = level;
    }
}

void Solution::refreshAll()
{
    C = 0;
    Qtot = 0;
    for (int i = 0; i < in->n; ++i)
        std::fill(veh[i].begin(), veh[i].end(), -1);
    for (int t = 0; t < in->H; ++t)
    {
        del[t] = 0;
        for (int i = 0; i < in->n; ++i)
            del[t] += q[i][t];
        Qtot += del[t];
        for (int k = 0; k < in->K; ++k)
        {
            rc[t][k] = 0;
            refreshRoute(t, k);
        }
    }
    for (int i = 0; i < in->n; ++i)
        refreshCustomer(i);
    refreshSupplier();
}

void Solution::setQ(int i, int t, double value)
{
    double change = value - q[i][t];
    q[i][t] = value;
    del[t] += change;
    Qtot += change;
}

int Solution::posOf(int i, int t) const
{
    int k = veh[i][t];
    if (k < 0)
        return -1;
    const vector<int> &r = R[t][k];
    for (int p = 0; p < static_cast<int>(r.size()); ++p)
        if (r[p] == i)
            return p;
    return -1;
}

// End-of-period holding cost over periods 1..H (supplier and customers).
double Solution::holding() const
{
    double total = 0;
    for (int t = 0; t < in->H; ++t)
    {
        total += in->h0 * S[t];
        for (int i = 0; i < in->n; ++i)
            total += in->h[i] * I[i][t];
    }
    return total;
}

double Solution::objective(Objective o) const
{
    if (o == Objective::LR)
        return logisticRatio();
    return C + holding();
}

string Solution::check() const
{
    std::ostringstream err;
    const Instance &ins = *in;
    const double tol = 1e-5;
    double cost = 0, quantity = 0;
    vector<vector<int>> seen(ins.n, vector<int>(ins.H, 0));
    for (int t = 0; t < ins.H; ++t)
    {
        for (int k = 0; k < ins.K; ++k)
        {
            double l = 0;
            const vector<int> &r = R[t][k];
            cost += routeCost(ins, r);
            for (int i : r)
            {
                if (i < 0 || i >= ins.n)
                    return "invalid customer index";
                seen[i][t]++;
                l += q[i][t];
                if (q[i][t] <= tol)
                    err << "customer " << i << " visited with zero quantity in period " << t << "; ";
                if (veh[i][t] != k)
                    err << "veh map wrong for " << i << "," << t << "; ";
            }
            if (l > ins.Q + tol)
                err << "vehicle " << k << " overloaded in period " << t << "; ";
        }
    }
    for (int i = 0; i < ins.n; ++i)
    {
        double level = ins.I0[i];
        for (int t = 0; t < ins.H; ++t)
        {
            if (seen[i][t] > 1)
                err << "customer " << i << " visited twice in period " << t << "; ";
            if (seen[i][t] == 0 && std::fabs(q[i][t]) > tol)
                err << "customer " << i << " receives without visit in period " << t << "; ";
            if (q[i][t] < -tol)
                err << "negative quantity; ";
            if (level + q[i][t] > ins.U[i] + tol)
                err << "customer " << i << " exceeds capacity in period " << t << "; ";
            level += q[i][t] - ins.d[i];
            if (level < -tol)
                err << "customer " << i << " stockout in period " << t << "; ";
            quantity += q[i][t];
        }
    }
    double supplier = ins.I00;
    for (int t = 0; t < ins.H; ++t)
    {
        double out = 0;
        for (int i = 0; i < ins.n; ++i)
            out += q[i][t];
        supplier += ins.r - out;
        if (supplier < -tol)
            err << "supplier stockout in period " << t << "; ";
    }
    if (std::fabs(cost - C) > 1e-4)
        err << "routing cost mismatch (" << cost << " vs " << C << "); ";
    if (std::fabs(quantity - Qtot) > 1e-4)
        err << "quantity mismatch (" << quantity << " vs " << Qtot << "); ";
    return err.str();
}
