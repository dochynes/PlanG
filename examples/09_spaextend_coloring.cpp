#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    
    App::setVertices(4);
    App::setTriangleFree(); // => spaextend

    App::setColorClassBounds({{3, 3}, {1, 1}}); // Equivalent to App::setRootedVertices(1) for n = 4:
    App::setCanonicalLabeling();


    return App::run();
}
