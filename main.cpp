
#include <iostream>
#include "geng/GengAPI.hpp"

#include "geng/GraphViewBoost.hpp"
#include "geng/GraphViewFunctions.hpp"

#include "common/Generator.hpp"

#include "boost/graph/sequential_vertex_coloring.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp> 
#include <boost/range/iterator_range.hpp>


using App = Generator<geng::Backend>;

const int MAX_DEG = 3;      
const int MAX_COLORS = 3;   

int main(int argc, char** argv)
{
    App::setPreprune([=](const auto& g)
    {
        auto last_v=num_vertices(g) - 1;

        if (out_degree(last_v, g) > MAX_DEG) 
        {
            return 1;
        }

        auto [neighbor_begin, neighbor_end] = adjacent_vertices(last_v, g); //TODO optimalizovat adj_vert

        for (auto it = neighbor_begin; it != neighbor_end; ++it) 
        {
            auto neighbor = *it;
        
        
            if (out_degree(neighbor, g) > MAX_DEG) 
            {
                return 1;
            }
        }
        return 0;


    });


    App::setPrune([=](const auto& g){


        std::size_t nv = num_vertices(g);
        std::vector<int> color_vec(nv);


        auto color_map = boost::make_iterator_property_map(color_vec.begin(), boost::identity_property_map());
        auto num_colors = boost::sequential_vertex_coloring(g, color_map);  //

        if(num_colors <= MAX_COLORS) 
        {
            return 0;
        }
        
        return 1;

    });

    App::setOutproc([](Output& out, const auto& g) 
    {
        out << g;

    });


    return App::run(argc,argv);
}