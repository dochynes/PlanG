#pragma once
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <vector>
extern "C" {
#include "nauty.h"   //  typ graph
}

namespace geng {

// View na graf ulozeny ve formatu nauty/geng
// drzi ukazatele predane z aktualniho behu
// generatoru + definuje typy a iteratory potrebne pro Boost.Graph
struct GraphView 
{
    const graph* g_;             // matice sousednosti ve formatu nauty
    int n_;                      // aktualni pocet vrcholu
    int maxn_;                   // cilovy pocet vrcholu pro dany beh gengu
    const int* colors_ = nullptr; // volitelne barvy vrcholu
    int color_count_ = 0;         // pocet pouzitych barev
        
    int num_vertices() const
    { 
        return n_;
    }
    int maxn() const 
    { 
        return maxn_; 
    }

    // pocet slov typu set v jednom radku nauty matice.
    int m() const 
    { 
        return SETWORDSNEEDED(maxn_); 
    }

    const graph* data() const
    {
         return g_; 
    }

    bool has_coloring() const
    {
        return colors_ != nullptr && color_count_ > 0;
    }

    int color_count() const
    {
        return color_count_;
    }

    int color(int v) const
    {
        if (!colors_ || v < 0 || v >= n_) 
            return -1;
        return colors_[v];
    }

    std::vector<int> vertex_colors() const
    {
        if (!colors_)
            return std::vector<int>(static_cast<std::size_t>(n_), -1);

        return std::vector<int>(colors_, colors_ + n_);
    }

    std::vector<int> vertices_of_color(int color) const
    {
        std::vector<int> vertices;

        if (!colors_ || color < 0)
            return vertices;

        for (int v = 0; v < n_; ++v)
            if (colors_[v] == color)
                vertices.push_back(v);

        return vertices;
    }

    // Typy vyzadovane boost::graph_traits<GraphView>.
    using vertex_descriptor = int;
    using edge_descriptor = std::pair<int,int>;
    using directed_category = boost::undirected_tag;
    using edge_parallel_category = boost::disallow_parallel_edge_tag;

    struct traversal_category: boost::vertex_list_graph_tag, boost::adjacency_graph_tag, boost::incidence_graph_tag
    {};
    using traversal_category = traversal_category;

    
    using vertices_size_type = std::size_t;
    using degree_size_type = std::size_t;
    using edges_size_type = std::size_t;

    using vertex_iterator = boost::counting_iterator<int>;
    

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
                    break;
            }

        }

        private:
            friend class boost::iterator_core_access;

    };
    using out_edge_iterator = out_edge_iterator;

    
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

    using adjacency_iterator = adjacency_iterator;

    
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
                if(v<=u)
                    v=u+1;

                const set* row = GRAPHROW(g->data(), u, g->m());

                for (;v<n;v++)
                {
                    if(ISELEMENT(row,v))
                    {
                        return;
                    }
                }

                v = u + 1;
        
            }
            v = 0;
        }

    };

    using edge_iterator = edge_iterator;

    static vertex_descriptor null_vertex() 
    {
        return -1;
    }
};

} 
