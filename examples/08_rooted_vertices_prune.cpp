#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    App app;

    // Generate graphs on 7 vertices with two rooted vertices and list 
    //only those where these two special vertices are not neighbors, but have a common neighbor
    app.setVertices(7);
    app.setRootedVertices(2);
    app.setCanonicalLabeling();

    app.setPrune([](const App::GraphView& g) {
        if (g.num_vertices() != g.maxn())
            return KEEP;

        int root1 = -1;
        int root2 = -1;

        for (int v = 0; v < g.num_vertices(); ++v)
        {
            if (g.color(v) == 1)
                root1 = v;
            else if (g.color(v) == 2)
                root2 = v;
        }

        if (root1 < 0 || root2 < 0)
            return PRUNE;


        if(App::distance_between(g,root1,root2)==2)
        {
            return KEEP;
        }
        else
        {
            return PRUNE;
        }
    });



    return app.run();
}
