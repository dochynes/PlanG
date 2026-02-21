#include "MyGraphLib.hpp"
#include <iostream>
#include <vector>


using App = Generator<plantri::Backend>;

int main(int argc, char** argv) 
{

    App::setVertices(12);

    // Search for graphs that require at least 4 colors
    App::setPrune([=](const auto& g){
        
        // 1. Prepare Boost structures
        std::vector<int> colors(num_vertices(g));
        auto color_map = boost::make_iterator_property_map(
            colors.begin(), boost::identity_property_map()
        );

        // 2. Call standard Boost algorithm
        // The sequential_vertex_coloring function accepts 'g' thanks to our Traits
        int num_colors = boost::sequential_vertex_coloring(g, color_map);

        // 3. Decision (Prune/Keep)
        if (num_colors >= 4) {
            return KEEP;
        }
        
        return PRUNE; 
    });

    App::setOutproc([](auto& out, const auto& g) 
    {
        out << "Found graph with chrom. number >= 4:";
        out << g;
    });

    return App::run();
}