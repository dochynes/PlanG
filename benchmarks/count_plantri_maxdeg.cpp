#include "MyGraphLib.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>

using App = Generator<plantri::Backend>;

namespace {

constexpr int MAX_DEG = 7;

bool commonedge(int a, int b, const auto& g)
{
    auto [begin, end] = out_edges(a, g);
    for(auto it = begin; it != end; ++it)
    {
        auto e = *it;
        auto e_inv = opposite_edge(e);
        auto e_temp = next_edge(e_inv);
        e_temp = next_edge(e_temp);
        if (target(e_temp, g) == b)
            return true;
    }
    return false;
}

// logic from the original plantri maxdeg.c plugin.
void install_maxdeg_callbacks(App& app)
{
    app.setPreprune([](const App::GraphView& g) {
        const int target_nv = maxnv(g);
        const int current_nv = num_vertices(g);
        const int levs = target_nv - current_nv;
        int excess = 0;
        int d3 = 0;
        int d4 = 0;
        int d3a = -1;
        int d3b = -1;

        for(int i = 0; i < current_nv; ++i)
        {
            const int d = out_degree(i, g);
            if(d == 3)
            {
                ++d3;
                d3a = d3b;
                d3b = i;
            }
            else if(d == 4)
            {
                ++d4;
            }
            else if(d > MAX_DEG)
            {
                excess += d - MAX_DEG;
            }
        }

        if(excess == 0) return KEEP;
        if(d3 > 2) return PRUNE;
        if(d3 == 2 && !commonedge(d3a, d3b, g)) return PRUNE;
        if(d3 > 0 && excess >= levs) return PRUNE;

        const int i_calc = d3 + d3 + d4;
        if(i_calc > 0 && excess > levs - i_calc + 2) return PRUNE;

        return KEEP;
    });

    app.setFilter([](const App::GraphView& g) {
        const int nv = num_vertices(g);
        for(int i = 0; i < nv; ++i)
            if(out_degree(i, g) > MAX_DEG)
                return PRUNE;
        return KEEP;
    });
}

} // namespace

int main(int argc, char** argv)
{
    if(argc < 2)
        return 2;

    std::uint64_t count = 0;
    App app;
    app.setVertices(std::atoi(argv[1]));
    app.setNoOutput();
    install_maxdeg_callbacks(app);
    app.setOutproc([&](Output&, const App::GraphView&) {
        ++count;
    });

    const int result = app.run();
    std::cout << count << '\n';
    return result;
}
