#include "MyGraphLib.hpp"

using App = Generator<geng::Backend>;

int main(int argc, char** argv)
{
    App app;

    // Triangle-free generation uses the spaextend branch inside geng.
    app.setVertices(4);
    app.setTriangleFree();
    app.setRootedVertices(1);
    //app.setCanonicalLabeling();


    return app.run();
}
