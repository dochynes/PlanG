#pragma once

#include "plantri/GraphView.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost
{

template<>
struct graph_traits<plantri::GraphView>
{

    using vertex_descriptor = int;
    using edge_descriptor = plantri::edge;//const EDGE*; // kvuli paralelnim hranam
    using directed_category = undirected_tag;
    using edge_parallel_category = allow_parallel_edge_tag;

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
        EDGE* e; // orient hrana z u
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

            if(u >= g->nv)
            {
                u=g->nv;
                e=nullptr;
                remaining=0;
                return;
            }


            //preskocime missing vertex
            if(g->missing_vertex>=0 &&  u==g->missing_vertex)
            {
                u++;
                if(u>=g->nv)
                {
                    u=g->nv;
                    e=nullptr;
                    remaining = 0;
                    return;
                }
            }


            while(u < g->nv)
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
            u=g->nv; 
            e = nullptr;
            remaining = 0;
        }

        bool is_canonical(EDGE* x)
        {
            return x && (x==x->min); 
        }

        void skip_non_canonical()
        {
            if(g)
            {
                while(u< g->nv && e)
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
            if(u >= g->nv || !e || remaining <=0)  
            {
                u=g->nv;
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


            if (v >= g->nv) 
            {
                v = g->nv;
                return;
            }

            if(g->missing_vertex >=0 && v == g->missing_vertex)
            {
                v++;
                if(v>=g->nv)
                    v=g->nv;
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
        EDGE* e;
        int remaining;

        adjacency_iterator(): g(nullptr),u(0),e(nullptr),remaining(0)
        {

        }

        adjacency_iterator(const plantri::GraphView* gg,int uu, EDGE* e0, int deg):g(gg),u(uu),e(e0),remaining(deg)
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
        EDGE* e;
        int remaining;

        out_edge_iterator() : g(nullptr), u(0), e(nullptr), remaining(0)
        {

        }

        out_edge_iterator(const plantri::GraphView* gg,int uu, EDGE* e0, int deg):g(gg),u(uu), e(e0),remaining(deg)
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


    };




};

