namespace geng
{

    using Traits = boost::graph_traits<GraphView>;




    inline std::pair<Traits::vertex_iterator,Traits::vertex_iterator> vertices(const GraphView& g)
    {
        return {Traits::vertex_iterator(0), Traits::vertex_iterator(g.num_vertices())};
    }

    inline Traits::vertices_size_type num_vertices(const GraphView& g)
    {
         return static_cast<Traits::vertices_size_type>(g.num_vertices());
    }

    inline std::pair<Traits::out_edge_iterator,Traits::out_edge_iterator> out_edges(Traits::vertex_descriptor u, const GraphView& g)
    {
        return {Traits::out_edge_iterator(u,0,&g), Traits::out_edge_iterator(u,g.num_vertices(),&g)};
    }

    inline Traits::degree_size_type out_degree(Traits::vertex_descriptor u, const GraphView& g)
    {

        const set* row = GRAPHROW(g.data(), u, g.m());
        std::size_t deg = 0;
        auto m = g.m();
        for (int i = 0; i < m; ++i) 
        {
            deg += POPCOUNT(row[i]);
        }

        return static_cast<Traits::degree_size_type>(deg);


        /*auto [first, last] = out_edges(u,g); //pomalejsi nez pres Popcount??? 
        std::size_t count = 0;
        for (auto it = first; it != last; ++it)
            ++count;
        return static_cast<Traits::degree_size_type>(count);
        */
    }


    inline std::pair<Traits::edge_iterator, Traits::edge_iterator> edges(const GraphView& g)
    {
        //begin u=1, v=1   end: u=n v=ibovolne
        return {Traits::edge_iterator(&g, 0, 1), Traits::edge_iterator(&g, g.num_vertices(), 0)};

    }

    inline Traits::edges_size_type num_edges(const GraphView& g)
    {
        auto [first, last] = edges(g);
        std::size_t count = 0;
        for (auto it = first; it != last; ++it)
            count++;
        
        return static_cast<Traits::edges_size_type>(count);

    }


    inline std::pair<Traits::adjacency_iterator,Traits::adjacency_iterator> adjacent_vertices(Traits::vertex_descriptor u, const GraphView& g)
    {
        return {Traits::adjacency_iterator(&g,u,0), Traits::adjacency_iterator(&g,u,g.num_vertices())};
    }


    inline Traits::vertex_descriptor source(Traits::edge_descriptor e, const GraphView&)
    {
        return e.first;
    }

    inline Traits::vertex_descriptor target(Traits::edge_descriptor e, const GraphView&)
    {
        return e.second;
    }





}