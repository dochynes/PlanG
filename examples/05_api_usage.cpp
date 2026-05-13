#include "MyGraphLib.hpp"
#include <iostream>

using App = Generator<geng::Backend>;


int main(int argc, char** argv)
{
    App app;

    //bipartite and claw-free graphs on 10 vertices that have between 10 and 15 edges, and save the output to a file in sparse6 format
    app.setVertices(10);
    app.setBipartite();
    app.setClawFree();
    app.setFormat(App::OutputFormat::Sparse6);
    app.setEdgeRange(10,15);
    app.setOutputFile("output.s6");

    // We can also use our custom filters, pre-filters, and output handlers here.

    return app.run();
}
