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
    using edges_size_type = std::size_t;

    using vertex_iterator = boost::counting_iterator<int>;
    //TODO ...

    //https://www.boost.org/doc/libs/latest/libs/iterator/doc/iterator_facade.html
    struct out_edge_iterator : boost::iterator_facade<out_edge_iterator,edge_descriptor,boost::forward_traversal_tag,edge_descriptor>
    {
        int u; // zdrojovy vrchol
        int v; // soused
        const geng::GraphView* g;

        out_edge_iterator(): u(0), v(0), g(nullptr)
        {

        }

        out_edge_iterator(int u,int v, const geng::GraphView* g) : u(u), v(v),g(g)
        {
            skip_non_neighbors();
        }

        edge_descriptor dereference() const  //&
        {
            return edge_descriptor{u, v};
        }

        void increment() 
        {
            v++;
            skip_non_neighbors();
        }
        
        bool equal(out_edge_iterator const& other) const 
        {
            return g == other.g && u == other.u && v == other.v;
        }

        void skip_non_neighbors()
        {
            if(!g) 
                return;

            for(;v<g->num_vertices();++v)
            {
                const set* row = GRAPHROW(g->data(), u, g->m());

                if(ISELEMENT(row, v))
                    break; // nasli jsme souseda
            }

        }

        private:
            friend class boost::iterator_core_access;

    };
    using out_edge_iterator = out_edge_iterator;
    //in_edge_iterator zatim ne, predpokladame neorientovany graf
    //adjacency_iterator  ne, muzeme pouzit out_edge_iterator
    //


    struct adjacency_iterator : boost::iterator_facade<adjacency_iterator,vertex_descriptor, boost::forward_traversal_tag, vertex_descriptor>
    {
        const geng::GraphView* g;
        int u;
        int v; //soused


        adjacency_iterator() : g(nullptr),u(0), v(0) 
        {

        }

        adjacency_iterator(const geng::GraphView* g, int u, int v) : g(g), u(u), v(v)
        {
            skip_non_neighbors();
        }

        vertex_descriptor dereference() const 
        {
            return v;
        }

        bool equal(adjacency_iterator const& other) const 
        {
            return g == other.g && u == other.u && v == other.v;
        }

        void increment() 
        {
            ++v;
            skip_non_neighbors();
        }

        private:
            friend class boost::iterator_core_access;
        
        void skip_non_neighbors()
        {
            if (!g) 
                return;
            int n= g->num_vertices();
            for(;v<n;++v)
            {
                const set* row = GRAPHROW(g->data(), u, g->m());
                if (ISELEMENT(row, v)) 
                {
                    break;
                }
            }
        }


    };

    struct edge_iterator : boost::iterator_facade<edge_iterator, edge_descriptor const, boost::forward_traversal_tag, edge_descriptor>
    {

        const geng::GraphView* g;
        int u; // prvni vrchol    
        int v; // druhy vrchol (u < v)

        edge_iterator():g(nullptr),u(0),v(0)
        {

        }

        edge_iterator(const geng::GraphView* g, int u, int v): g(g), u(u),v(v)
        {
            skip_nonedges();
        }

        edge_descriptor dereference() const 
        {
            return edge_descriptor{u, v};
        }

        void increment() 
        {
            v++;
            skip_nonedges();
        }

        bool equal(edge_iterator const& other) const 
        {
            return g == other.g && u == other.u && v == other.v;
        }

        private:
            friend class boost::iterator_core_access;


        void skip_nonedges()
        {
            if(!g)
                return;

            int n = g->num_vertices();

            for(;u<n;++u)
            {
                if(v<u)
                    v=u+1; // protoze musi platit v > u

                const set* row = GRAPHROW(g->data(), u, g->m());

                for (;v<n;v++)
                {
                    if(ISELEMENT(row,v))
                    {
                        return; //nasle jsme hranu
                    }
                }

                v = u + 1;
        
            }
            //konec (end iterator)
        }

    };

    using edge_iterator = edge_iterator;

    static vertex_descriptor null_vertex() 
    {
        return -1;
    }



};

}