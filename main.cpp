#include "MyGraphLib.hpp"
#include <iostream>

// A) General graph generation (nauty/geng)
// using App = common::Generator<geng::Backend>;

// B) Planar graph generation (plantri)
using App = Generator<plantri::Backend>;

int main(int argc, char** argv)
{
    // TODO: Your implementation goes here
    // (e.g. App::setPrune(...) or App::setOutproc(...))

    return App::run(argc, argv);
}