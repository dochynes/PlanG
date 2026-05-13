#include "MyGraphLib.hpp"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <iostream>
#include <vector>


using App = Generator<plantri::Backend>;

int main(int argc, char** argv) 
{
    App app;

    app.setVertices(13);

    // Search for graphs that require at least 4 colors
    app.setPrune([=](const auto& g){
        
        // 1. Prepare Boost structures. boost can use the std::vector directly as a property map.
        std::vector<int> colors(num_vertices(g));

        //Alternatively, we could explicitly construct a Boost property map: 
        /*
        auto color_map = boost::make_iterator_property_map(
            colors.begin(), boost::identity_property_map()
        );
        */

        // 2. Call standard Boost algorithm
        // The sequential_vertex_coloring function accepts 'g' thanks to our Traits
        int num_colors = boost::sequential_vertex_coloring(g, colors.data());

        // 3. Decision (Prune/Keep)
        if (num_colors >= 4) {
            return KEEP;
        }
        
        return PRUNE; 
    });

    app.setOutproc([](auto& out, const auto& g) 
    {
        out << "Found graph with chrom. number >= 4:";
        out << g;
    });

    return app.run();

    
}
