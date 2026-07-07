#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    App app;

    // Explicit color class on 5 vertices:
    // color 0 has size 3, color 1 has size 2.
    app.setVertices(5);
    app.setColorClassSizes({3, 2});
    app.setCanonicalLabeling();

    
    app.setFilter([](const App::GraphView& g){
        if (g.num_vertices() != g.maxn())
            return KEEP;

        int cross_edges = 0;
        auto [first, last] = edges(g);
        for (auto it = first; it != last; ++it)
        {
            const auto [u, v] = *it;
            if (g.color(u) != g.color(v))
                ++cross_edges;
        }

        return cross_edges >= 2 ? KEEP : PRUNE;
    });  
    // the filter will only leave graphs where there are at least two edges between these two groups

    return app.run();
}
