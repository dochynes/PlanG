#include "MyGraphLib.hpp"
#include <iostream>

using App = Generator<geng::Backend>;


int main(int argc, char** argv)
{
    //bipartite and claw-free graphs on 10 vertices that have between 10 and 15 edges, and save the output to a file in sparse6 format
    App::setVertices(10);
    App::setBipartite();
    App::setClawFree();
    App::setFormat(App::OutputFormat::Sparse6);
    App::setEdgeRange(10,15);
    App::setOutputFile("output.s6");

    // We can also use our custom filters, pre-filters, and output handlers here.

    return App::run();
}
