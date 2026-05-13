#include "MyGraphLib.hpp"

using App = Generator<plantri::Backend>;

int main()
{
    App app;

    app.setVertices(7);

    // disk triangulations can contain a missing raw vertex slot.
    // vertex_index maps the remaining raw descriptors to compact indices
    // usable for arrays/vectors of size num_vertices(g)
    app.setDiskSize(3);
    app.setHeader();

    app.setOutproc([](Output& out, const App::GraphView& g) {
        static bool printed = false;
        if (printed)
            return;
        printed = true;

        auto index = get(boost::vertex_index, g);

        out << "missing_vertex=" << g.missing_vertex << " num_vertices=" << static_cast<int>(num_vertices(g)) << "\n";
        out << "raw:index";

        auto [first, last] = vertices(g);
        for (auto it = first; it != last; ++it)
            out << " " << *it << ":" << get(index, *it);

        out << "\n";
    });

    return app.run();
}
