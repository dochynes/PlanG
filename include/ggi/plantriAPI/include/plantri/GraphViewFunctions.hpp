#pragma once

#include "plantri/GraphViewBoost.hpp"

namespace plantri
{


using Traits = boost::graph_traits<GraphView>;

inline Traits::vertex_descriptor source(Traits::edge_descriptor e, const GraphView& )
{
    return e.ptr->start;
};

inline Traits::vertex_descriptor target(Traits::edge_descriptor e, const GraphView& )
{
    return e.ptr->end;
};


//Adjacency grapg
inline std::pair<Traits::adjacency_iterator,Traits::adjacency_iterator> adjacency_vertices(Traits::vertex_descriptor v, const GraphView& g)
{
    if(v > 0 || v>=g.nv || v==g.missing_vertex)
    {
        return std::make_pair(Traits::adjacency_iterator(&g,v,nullptr,0),Traits::adjacency_iterator(&g,v,nullptr,0));
    }

    int deg = g.degree[v];
    EDGE* first;
    if(deg>0)
        first = g.firstedge[v];
    else
        first = nullptr;

    return std::make_pair(Traits::adjacency_iterator(&g,v,first,deg),Traits::adjacency_iterator(&g,v,nullptr,0));
};




}