#include "MyGraphLib.hpp"

#include <cstdlib>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 4)
        return 2;

    std::string backend = argv[1];
    int n = std::atoi(argv[2]);
    std::string output = argv[3];

    if (backend == "geng")
    {
        Generator<geng::Backend> app;
        app.setVertices(n);
        app.setQuiet();
        app.setOutputFile(output);
        return app.run();
    }

    if (backend == "plantri")
    {
        Generator<plantri::Backend> app;
        app.setVertices(n);
        app.setFormat(plantri::Backend::OutputFormat::Graph6);
        app.setOutputFile(output);
        return app.run();
    }

    return 2;
}
