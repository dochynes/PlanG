#pragma once

#include "plantri/GraphViewBoost.hpp"
#include <utility>

namespace plantri
{

// Globalni funkce tvorici rozhrani kompatibilni s Boost.Graph
using Traits = boost::graph_traits<GraphView>;

inline Traits::vertex_descriptor source(Traits::edge_descriptor e, const GraphView& )
{
    return e.ptr->start;
};

inline Traits::vertex_descriptor target(Traits::edge_descriptor e, const GraphView& )
{
    return e.ptr->end;
};


// IncidenceGraph: vrati orientovane hrany vychazejici z vrcholu u
inline std::pair<Traits::out_edge_iterator, Traits::out_edge_iterator> out_edges(Traits::vertex_descriptor u, const GraphView& g)
{
    if(!g.is_valid_vertex(u))
    {
        return std::make_pair(Traits::out_edge_iterator(&g,u,nullptr,0),Traits::out_edge_iterator(&g,u,nullptr,0));
    }

    int deg = g.degree[u];
    const EDGE* first;
    if(deg>0)
        first = g.firstedge[u];
    else
        first = nullptr;

    return std::make_pair(Traits::out_edge_iterator(&g,u,first,deg),Traits::out_edge_iterator(&g,u,nullptr,0));


};


// Stupen vrcholu
inline Traits::degree_size_type out_degree(Traits::vertex_descriptor u,const GraphView& g)
{
    if (!g.is_valid_vertex(u))
        return 0;
    return g.degree[u];
}


// VertexListGraph
inline std::pair<Traits::vertex_iterator, Traits::vertex_iterator> vertices(const GraphView& g)
{
    return std::make_pair(Traits::vertex_iterator(&g,0),Traits::vertex_iterator(&g,g.vertex_limit()));
}

// Pocet platnych vrcholu aktualniho grafu.
inline auto num_vertices(const GraphView& g)
{
    return g.num_vertices();
}

// EdgeListGraph: iterator vraci kazdou neorientovanou hranu jen jednou
inline std::pair<Traits::edge_iterator, Traits::edge_iterator> edges(const GraphView& g)
{
    return std::make_pair(Traits::edge_iterator(&g,0),Traits::edge_iterator(&g,g.vertex_limit()));
}

// Pocet neorientovanych hran
inline auto num_edges(const GraphView& g)
{
    return g.num_edges();
}

// AdjacencyGraph: sousedi vrcholu u v cyklickem poradi ulozenem plantri
inline std::pair<Traits::adjacency_iterator, Traits::adjacency_iterator> adjacent_vertices(Traits::vertex_descriptor u, const GraphView& g)
{

    if(!g.is_valid_vertex(u))
    {
        return std::make_pair(Traits::adjacency_iterator(&g, u, nullptr, 0), Traits::adjacency_iterator(&g, u, nullptr, 0));

    }

    int deg = g.degree[u];
    const EDGE* start_edge = nullptr;

    if (deg > 0) 
    {
        start_edge = g.firstedge[u];
    }

    return std::make_pair(Traits::adjacency_iterator(&g, u, start_edge, deg), Traits::adjacency_iterator(&g, u, nullptr, 0));

}


// Pomocne funkce pro navigaci v half-edge strukture plantri.
inline Traits::edge_descriptor opposite_edge(Traits::edge_descriptor e)
{
    return {e.ptr->invers};
}

inline Traits::edge_descriptor next_edge(Traits::edge_descriptor e)
{
    return {e.ptr->next};
}

inline Traits::edge_descriptor prev_edge(Traits::edge_descriptor e)
{
    return {e.ptr->prev};
}


// Ciliovy pocet vrcholu pozadovany aktualnim spustenim plantri.
inline int maxnv(const GraphView& g)
{
    return g.maxnv;
}

// Property map pro Boost.Graph algoritmy. Umi zkompaktovat indexy, kdyz ma
// plantri v internim poli "missing vertex"
inline vertex_index_map get(boost::vertex_index_t,const GraphView& g)
{
    return vertex_index_map{g.missing_vertex};
}


}
