// Construction heuristic, AMM s3.1 and Algorithm 1.
// The logic follows ConstructAMMInitialSolution in ../Heuristic/src/InitialWithZone.cpp
// (used by the MSILS-Initial-Alt variant), so both codes build identical initial solutions.
#include "amm.h"

namespace
{
// Nearest-neighbour routing of the selected customers (AMM s3.1, Braysy & Gendreau 2005).
// Vehicles are filled one after another: starting from the last node of the vehicle's
// route (the depot if empty), the nearest unrouted customer whose delivery fits the
// residual capacity is appended. Deliveries are also limited by the supplier's stock.
// Returns true if every customer in the list was routed.
bool routeNearestNeighbour(const Instance &in, Solution &s, int t, const vector<std::pair<int, double>> &customers,
                           double &availableSupply, vector<double> &inventory, vector<double> &vehicleLoad)
{
    vector<bool> routed(customers.size(), false);
    size_t numberRouted = 0;
    for (int k = 0; k < in.K && numberRouted < customers.size(); ++k)
    {
        vector<int> &route = s.R[t][k];
        int current = route.empty() ? 0 : route.back() + 1;
        while (numberRouted < customers.size())
        {
            int nearest = -1;
            double nearestDist = INF, nearestQty = 0;
            for (size_t x = 0; x < customers.size(); ++x)
            {
                if (routed[x])
                    continue;
                double qty = std::min(customers[x].second, availableSupply);
                if (qty <= 1e-9 || vehicleLoad[k] + qty > in.Q + 1e-9)
                    continue;
                double dist = in.c[current][customers[x].first + 1];
                if (dist < nearestDist)
                    nearestDist = dist, nearest = static_cast<int>(x), nearestQty = qty;
            }
            if (nearest < 0)
                break;
            int i = customers[nearest].first;
            route.push_back(i);
            s.q[i][t] = nearestQty;
            vehicleLoad[k] += nearestQty;
            availableSupply -= nearestQty;
            inventory[i] += nearestQty;
            routed[nearest] = true;
            ++numberRouted;
            current = i + 1;
        }
    }
    return numberRouted == customers.size();
}
} // namespace

bool construct(Context &cx, Solution &best)
{
    const Instance &in = cx.in;
    const double eps = 1e-9;
    const int maxLookAhead = (in.H + 1) / 2; // ceil(T/2)
    bool found = false;
    double bestF = INF;

    for (int ratioStep = 10; ratioStep > 0; --ratioStep)
    {
        const double ratioDemand = ratioStep / 10.0;
        for (int lookAhead = 1; lookAhead <= maxLookAhead; ++lookAhead)
        {
            Solution s;
            s.init(in);
            vector<double> inventory(in.I0);
            double supplier = in.I00;
            bool feasible = true;
            for (int t = 0; t < in.H && feasible; ++t)
            {
                vector<std::pair<int, double>> mandatory, optional;
                const bool lastPeriod = (t == in.H - 1);
                for (int i = 0; i < in.n; ++i)
                {
                    const double withoutDelivery = inventory[i] - in.d[i];
                    if (withoutDelivery < -eps)
                    {
                        // eq. (3) in the last period, eq. (4) otherwise
                        double qty = lastPeriod ? std::min(-withoutDelivery, in.Q)
                                                : std::min(in.U[i] - inventory[i], in.Q);
                        mandatory.push_back({i, std::max(0.0, qty)});
                        continue;
                    }
                    // criterion 1: stockout within the next look_ahead periods
                    bool urgent = false;
                    for (int off = 1; off <= lookAhead && t + off < in.H; ++off)
                        if (inventory[i] - in.d[i] * (off + 1) < -eps)
                        {
                            urgent = true;
                            break;
                        }
                    double qty = std::min((in.U[i] - inventory[i]) * ratioDemand, in.Q);
                    // criterion 2 (last period): q h0 > q hi + 2 c0i
                    bool economical = lastPeriod && qty * in.h0 > qty * in.h[i] + 2.0 * in.c[0][i + 1];
                    if (qty > eps && (urgent || economical))
                        optional.push_back({i, qty});
                }

                double available = supplier + in.r;
                vector<double> vehicleLoad(in.K, 0.0);
                if (!routeNearestNeighbour(in, s, t, mandatory, available, inventory, vehicleLoad))
                {
                    feasible = false;
                    break;
                }
                routeNearestNeighbour(in, s, t, optional, available, inventory, vehicleLoad);
                supplier = available;
                if (supplier < -eps)
                    feasible = false;
                for (int i = 0; i < in.n; ++i)
                {
                    inventory[i] -= in.d[i];
                    if (inventory[i] < -eps)
                        feasible = false;
                }
            }
            if (!feasible)
                continue;
            s.refreshAll();
            if (!s.check().empty() || s.Qtot <= eps)
                continue;
            double f = cx.f(s);
            if (!found || f < bestF)
            {
                best = s;
                bestF = f;
                found = true;
            }
        }
    }
    return found;
}
