#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    // Triangle-free generation uses the spaextend branch inside geng.
    App::setVertices(4);
    App::setTriangleFree();
    App::setRootedVertices(1);
    //App::setCanonicalLabeling();


    return App::run();
}
