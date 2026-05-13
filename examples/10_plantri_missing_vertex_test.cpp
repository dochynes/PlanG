#include "MyGraphLib.hpp"

using App = Generator<plantri::Backend>;

int main()
{
    App app;

    app.setVertices(7);

    // disk triangulations are one of the Plantri modes where missing_vertex
    // can appear. This example checks that the API skips that raw slot and
    // still iterates over every valid vertex descriptor correctly.
    app.setDiskSize(3);
    app.setFormat(App::OutputFormat::Graph6);
    app.setHeader();

    app.setOutproc([](Output& out, const App::GraphView& g) {
        static bool printed = false;
        if (printed)
            return;
        printed = true;

        out << " missing_vertex=" << g.missing_vertex << " num_vertices=" << num_vertices(g) << "\n";
        out << "vertices: ";

        auto [first, last] = vertices(g);
        for (auto it = first; it != last; ++it)
            out << " " << *it;

        out << "\n" << g;
    });

    return app.run();
}
