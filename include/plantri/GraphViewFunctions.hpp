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
    if(v < 0 || v>=g.nv || v==g.missing_vertex)
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

//incidence graph
inline auto out_edges(Traits::vertex_descriptor u, const GraphView& g)
{
    if(u < 0 || u>=g.nv || u==g.missing_vertex)
    {
        return std::make_pair(Traits::out_edge_iterator(&g,u,nullptr,0),Traits::out_edge_iterator(&g,u,nullptr,0));
    }

    int deg = g.degree[u];
    EDGE* first;
    if(deg>0)
        first = g.firstedge[u];
    else
        first = nullptr;

    return std::make_pair(Traits::out_edge_iterator(&g,u,first,deg),Traits::out_edge_iterator(&g,u,nullptr,0));


};


inline Traits::degree_size_type out_degree(Traits::vertex_descriptor u,const GraphView& g)
{
    if (u < 0 || u >= g.nv || u == g.missing_vertex) 
        return 0;
    return g.degree[u];
}


inline auto vertices(const GraphView& g)
{
    return std::make_pair(Traits::vertex_iterator(&g,0),Traits::vertex_iterator(&g,g.nv));
}

inline auto num_vertices(const GraphView& g)
{
    return g.nv - (g.missing_vertex >= 0 ? 1 : 0);
}

inline auto edges(const GraphView& g)
{
    return std::make_pair(Traits::edge_iterator(&g,0),Traits::edge_iterator(&g,g.nv));
}

inline auto num_edges(const GraphView& g)
{
    return g.num_edges();
}



}