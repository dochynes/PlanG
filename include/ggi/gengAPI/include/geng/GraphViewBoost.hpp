#pragma once
#include "geng/GraphView.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>


namespace boost 
{

//https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_traits.html
template<>
struct graph_traits<geng::GraphView>
{
    using vertex_descriptor = int;
    using edge_descriptor = std::pair<int,int>;
    using directed_category = undirected_tag;  // nebo directed_tag
    using edge_parallel_category = disallow_parallel_edge_tag; // nebo allow...
    using traversal_category = adjacency_graph_tag;
    using vertices_size_type = std::size_t;
    using degree_size_type = std::size_t;

    using vertex_iterator = boost::counting_iterator<int>;
    //TODO ...

    //struct out_edge_iterator :



};

}