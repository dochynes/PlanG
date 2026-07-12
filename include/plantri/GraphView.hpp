#pragma once
//#include "plantri/BridgeAPI.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/iterator_range.hpp>
#include <cstddef>
#include <utility>
namespace plantri
{

    struct edge
    {
        const EDGE* ptr = nullptr;

        bool operator==(const edge& other) const
        {
            return ptr == other.ptr;
        }

        bool operator!=(const edge& other) const
        {
            return ptr != other.ptr;
        }


    };

    struct GraphView
    {

        const EDGE* const* firstedge;
        const int* degree;
        int nv;
        int ne_oriented;
        int missing_vertex;
        int maxnv;

        int num_vertices() const 
        {
            return nv;
        }
        int num_edges() const 
        {
            return ne_oriented / 2; 
        }

         using vertex_descriptor = int;

    static vertex_descriptor null_vertex() 
    {
        return -1;
    }
    using edge_descriptor = plantri::edge;//const EDGE*; // kvuli paralelnim hranam
    using directed_category = boost::undirected_tag;
    using edge_parallel_category = boost::allow_parallel_edge_tag;

    using vertices_size_type = std::size_t;
    using degree_size_type = std::size_t;
    using edges_size_type = std::size_t;

    struct traversal_category: boost::vertex_list_graph_tag, boost::edge_list_graph_tag, boost::adjacency_graph_tag, boost::incidence_graph_tag
    {};

    using traversal_category = traversal_category;


    struct edge_iterator : boost::iterator_facade<edge_iterator,edge_descriptor const,boost::forward_traversal_tag, edge_descriptor>
    {
        const plantri::GraphView* g;
        int u; //start
        const EDGE* e; // orient hrana z u
        int remaining; // kolik hran zbyva jeste projit

        edge_iterator():g(nullptr),u(0),e(nullptr),remaining(0)
        {

        }

        edge_iterator(const plantri::GraphView* g,int u) :g(g), u(u),e(nullptr),remaining(0)
        {
            init_at_u();
            skip_non_canonical();
        }
        friend class boost::iterator_core_access;



        void init_at_u()
        {
            if(!g)
            {
                u=0;
                e=nullptr;
                remaining=0;
                return;
            }

            const int limit = g->vertex_limit();

            if(u >= limit)
            {
                u=limit;
                e=nullptr;
                remaining=0;
                return;
            }


            //preskocime missing vertex
            if(g->missing_vertex>=0 &&  u==g->missing_vertex)
            {
                u++;
                if(u>=limit)
                {
                    u=limit;
                    e=nullptr;
                    remaining = 0;
                    return;
                }
            }


            while(u < limit)
            {
                if(g->missing_vertex>=0 &&  u==g->missing_vertex)
                {
                    u++;
                    continue;
                }

                int deg = g->degree[u];

                if(deg>0)
                {
                    remaining= deg;



                    e = g->firstedge[u];
                    return;
                }
                u++;

            }

            //konec
            u=limit;
            e = nullptr;
            remaining = 0;
        }

        bool is_canonical(const EDGE* x)
        {
            return x && (x==x->min); 
        }

        void skip_non_canonical()
        {
            if(g)
            {
                while(u< g->vertex_limit() && e)
                {
                    if(is_canonical(e))
                    {
                        return;
                    }
                    move_next_internal();
                }
            }


        }

        void move_next_internal()
        {
            const int limit = g ? g->vertex_limit() : 0;

            if(u >= limit || !e || remaining <=0)
            {
                u=limit;
                e = nullptr;
                remaining = 0;
                return;
            }

            if( remaining > 1) //dalsi hrana u stejneho vrcholu
            {
                e=e->next;
                remaining--;
                return;
            }

            u++;
            init_at_u();
        }

        edge_descriptor dereference()
        {
            return edge_descriptor { e };
        }

        void increment()
        {
            move_next_internal();
            skip_non_canonical();
        }

        bool equal(edge_iterator const& other) const
        {
            return g == other.g && u == other.u && e == other.e && remaining == other.remaining;
        }
        
    };

    using edge_iterator = edge_iterator;


    struct vertex_iterator: boost::iterator_facade< vertex_iterator, vertex_descriptor, boost::forward_traversal_tag, vertex_descriptor>  
    {
        const plantri::GraphView* g;
        int v;

        vertex_iterator(): g(nullptr),v(0)
        {

        }

        vertex_iterator(const plantri::GraphView* gg, int vv):g(gg), v(vv)
        {
            skip_missing();
        }

        friend class boost::iterator_core_access;

        void skip_missing()
        {
            if(!g)
                return;


            const int limit = g->vertex_limit();

            if (v >= limit)
            {
                v = limit;
                return;
            }

            if(g->missing_vertex >=0 && v == g->missing_vertex)
            {
                v++;
                if(v>=limit)
                    v=limit;
            }
        }

        vertex_descriptor dereference() const
        {
            return v;
        }

        void increment()
        {
            v++;
            skip_missing();
        }

        bool equal(vertex_iterator const& other) const
        {
            return g == other.g && v == other.v;
        }

    };

    using vertex_iterator = vertex_iterator;

    struct adjacency_iterator : boost::iterator_facade<adjacency_iterator,vertex_descriptor,boost::forward_traversal_tag,vertex_descriptor>
    {
        const plantri::GraphView* g;
        int u;
        const EDGE* e;
        int remaining;

        adjacency_iterator(): g(nullptr),u(0),e(nullptr),remaining(0)
        {

        }

        adjacency_iterator(const plantri::GraphView* gg,int uu, const EDGE* e0, int deg):g(gg),u(uu),e(e0),remaining(deg)
        {

        }
        friend class boost::iterator_core_access;

        vertex_descriptor dereference() const
        {
            return e->end;
        }

        void increment()
        {
            if (!g || !e || remaining <= 0) 
            {
                e = nullptr;
                remaining = 0;
                return;
            }

            if(remaining > 1)
            {
                e=e->next;
                remaining--;
            }
            else
            {
                e = nullptr;
                remaining=0;
            }
        }

        bool equal(adjacency_iterator const& other) const
        {
            if (g != other.g || u != other.u) 
                return false;

            bool this_end  = (!e || remaining == 0);
            bool other_end = (!other.e || other.remaining == 0);

            if (this_end || other_end)
                return this_end == other_end;

            return e == other.e;

        }

    };
    using adjacency_iterator = adjacency_iterator;


    struct out_edge_iterator : boost::iterator_facade<out_edge_iterator, edge_descriptor,boost::forward_traversal_tag, edge_descriptor>
    {
        const plantri::GraphView* g;
        int u; 
        const EDGE* e;
        int remaining;

        out_edge_iterator() : g(nullptr), u(0), e(nullptr), remaining(0)
        {

        }

        out_edge_iterator(const plantri::GraphView* gg,int uu, const EDGE* e0, int deg):g(gg),u(uu), e(e0),remaining(deg)
        {

        }

        friend class boost::iterator_core_access;

        edge_descriptor dereference() const
        {
            return edge_descriptor{e};
        }

        void increment()
        {
            if (!g || !e || remaining <= 0) 
            {
                e = nullptr;
                remaining = 0;
                return;
            }

            if (remaining > 1) 
            {
                e = e->next;
                --remaining;
            } 
            else 
            {
                e = nullptr;
                remaining = 0;
            }
        }


        bool equal(out_edge_iterator const& other) const
        {
            if (g != other.g || u != other.u) 
                return false;

            bool this_end  = (!e || remaining == 0);
            bool other_end = (!other.e || other.remaining == 0);

            if (this_end || other_end)
                return this_end == other_end;

            return e == other.e;

        }

    };

    using out_edge_iterator = out_edge_iterator;

    friend std::pair<out_edge_iterator, out_edge_iterator> out_edges(vertex_descriptor u, const GraphView& g);
    friend degree_size_type out_degree(vertex_descriptor u, const GraphView& g);
    friend std::pair<vertex_iterator, vertex_iterator> vertices(const GraphView& g);
    friend std::pair<edge_iterator, edge_iterator> edges(const GraphView& g);
    friend std::pair<adjacency_iterator, adjacency_iterator> adjacent_vertices(vertex_descriptor u, const GraphView& g);

private:
    int vertex_limit() const
    {
        return nv + (missing_vertex >= 0 ? 1 : 0);
    }

    bool is_valid_vertex(int v) const
    {
        return v >= 0 && v < vertex_limit() && v != missing_vertex;
    }

    std::size_t vertex_index(int v) const
    {
        return static_cast<std::size_t>((missing_vertex >= 0 && v > missing_vertex) ? v - 1 : v);
    }
    };

    struct vertex_index_map
    {
        using key_type = GraphView::vertex_descriptor;
        using value_type = std::size_t;
        using reference = std::size_t;
        using category = boost::readable_property_map_tag;

        int missing_vertex = -1;
    };

    inline std::size_t get(vertex_index_map map, GraphView::vertex_descriptor v)
    {
        return static_cast<std::size_t>((map.missing_vertex >= 0 && v > map.missing_vertex) ? v - 1 : v);
    }
}
