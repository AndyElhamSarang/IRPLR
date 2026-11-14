#include "amm.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool Instance::read(const string &path, int vehiclesOverride)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Cannot open " << path << "\n";
        return false;
    }

    size_t slash = path.find_last_of("/\\");
    string base = path.substr(slash == string::npos ? 0 : slash + 1);
    size_t dot = base.rfind('.');
    name = dot == string::npos ? base : base.substr(0, dot);

    string line;
    std::getline(file, line);
    {
        std::istringstream ss(line);
        int nodes = 0;
        if (!(ss >> nodes >> H >> Q))
        {
            std::cerr << "Invalid header in " << path << "\n";
            return false;
        }
    }

    vector<double> x, y;
    std::getline(file, line);
    {
        std::istringstream ss(line);
        int id;
        double sx, sy;
        if (!(ss >> id >> sx >> sy >> I00 >> r >> h0))
        {
            std::cerr << "Invalid supplier line in " << path << "\n";
            return false;
        }
        x.push_back(sx);
        y.push_back(sy);
    }

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        int id;
        double cx, cy, i0, u, lmin, dem, hc;
        if (!(ss >> id >> cx >> cy >> i0 >> u >> lmin >> dem >> hc))
            continue; // blank or trailing line
        x.push_back(cx);
        y.push_back(cy);
        I0.push_back(i0);
        U.push_back(u);
        d.push_back(dem);
        h.push_back(hc);
    }
    n = static_cast<int>(I0.size());

    if (vehiclesOverride > 0)
        K = vehiclesOverride;
    else
    {
        size_t us = name.find_last_of('_');
        K = (us == string::npos) ? 1 : std::max(1, std::atoi(name.c_str() + us + 1));
    }

    c.assign(n + 1, vector<double>(n + 1, 0.0));
    for (int a = 0; a <= n; ++a)
        for (int b = 0; b <= n; ++b)
            if (a != b)
                c[a][b] = std::round(std::sqrt((x[a] - x[b]) * (x[a] - x[b]) + (y[a] - y[b]) * (y[a] - y[b])));
    return true;
}
