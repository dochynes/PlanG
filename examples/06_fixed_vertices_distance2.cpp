#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    // example: all anchored-isomorphism classes on 8 vertices
    // anchored vertices 0 and 1 are at distance == 2
    App::setVertices(8);
    App::setFixedVertices({0, 1});

    App::setOutproc([](Output& out, const App::GraphView& g) {
        if (App::distance_between(g, 0, 1) == 2)
            out << g;
    });

    return App::run();
}
