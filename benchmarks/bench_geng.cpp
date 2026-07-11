#include "MyGraphLib.hpp"

#include <cstdlib>
#include <string>

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    if (argc < 2)
        return 2;

    int n = std::atoi(argv[1]);
    std::string mode = "base";
    if (argc >= 3)
        mode = argv[2];

    App app;
    app.setVertices(n);
    app.setNoOutput();
    app.setQuiet();

    if(mode == "pass-filter")
    {
        app.setFilter([](const App::GraphView&) {
            return KEEP;
        });
    }
    else if(mode == "pass-preprune")
    {
        app.setPreprune([](const App::GraphView&) {
            return KEEP;
        });
    }
    else if(mode == "colored-balanced")
    {
        app.setColorClassSizes({n/2, n - n/2});
    }
    else if(mode == "rooted2")
    {
        app.setRootedVertices(2);
    }

    return app.run();
}
