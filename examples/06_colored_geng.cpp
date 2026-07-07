#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    App app;

    // Graphs on 4 vertices with exactly 2 labelled colors.
    // Internally this runs class sizes {1,3}, {2,2}, {3,1}.
    app.setVertices(4);
    app.setColors(2);
    app.setCanonicalLabeling();

    app.setFilter([](const App::GraphView& g) {
        if (g.num_vertices() != g.maxn())
            return KEEP;

        int color_zero_count = 0;

        for (int v = 0; v < g.num_vertices(); ++v)
        {
            if (g.color(v) == 0)
                ++color_zero_count;
        }

        // Keep only colorings where color 0 has exactly 2 vertices.
        if(color_zero_count == 2)
            return KEEP;
        return PRUNE;
    });

    return app.run();
}
