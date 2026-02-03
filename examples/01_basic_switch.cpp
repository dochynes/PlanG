#include "MyGraphLib.hpp"
#include <iostream>

// STEP 1: Select a backend.
// Uncomment the one you wish to use. The rest of the code remains unchanged.

// A) General graph generation (nauty/geng)
using App = common::Generator<geng::Backend>;

// B) Planar graph generation (plantri)
// using App = common::Generator<plantri::Backend>;

int main(int argc, char** argv) 
{
    std::cout << "API Example\n";

    // STEP 2(optional): Define the output
    // Here we use 'operator<<', which is implemented for both backends
    App::setOutproc([](auto& out, const auto& g) {
        out << g; 
    });

    // STEP 3: Execution
    // geng:    ./plang -c 5 (5 vertices, connected)
    // plantri: ./plang 10 (10 vertices, triangulations)
    return App::run(argc, argv);
}