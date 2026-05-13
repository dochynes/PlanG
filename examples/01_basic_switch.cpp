#include "MyGraphLib.hpp"
#include <iostream>

// STEP 1: Select a backend.
// Uncomment the one you wish to use. The rest of the code remains unchanged.

// A) General graph generation (nauty/geng)
using App = Generator<geng::Backend>;

// B) Planar graph generation (plantri)
// using App = Generator<plantri::Backend>;

int main(int argc, char** argv) 
{
    App app;

    std::cout << "API Example";
    // STEP 2: Configure generator parameters
    app.setVertices(11);
    app.setTriangleFree();

    // (optional): Define the output
    // Here we use 'operator<<', which is implemented for both backends
    app.setOutproc([](Output& out, const App::GraphView& g) {   // We can use 'auto' here thanks to type deduction.
        out << g; 
    });

    return app.run();
}
