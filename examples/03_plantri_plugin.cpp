/*
 * Usage: ./plang
 *
 * This will generate planar triangulations with n(14) vertices
 * and apply the max degree filter defined below.
 */
/// !!!Logic adapted from maxdeg.c!!!

#include "MyGraphLib.hpp"

using App = Generator<plantri::Backend>;

const int MAX_DEG = 7;      


// Test whether vertices a, b are at the opposite points of two adjacent faces
bool commonedge(int a, int b, const auto& g)
{
    auto [begin, end] = out_edges(a, g);

    for (auto it = begin; it != end; ++it)
    {
        auto e = *it; 
        auto e_inv = opposite_edge(e);
        auto e_temp = next_edge(e_inv);
        e_temp = next_edge(e_temp);

        if (target(e_temp, g) == b) 
        {
            return true;
        }
    }
    return false;   
}

int main(int argc, char** argv)
{
    App app;

    app.setVertices(14);
    // 1. PRE-PRUNE (Heuristic from maxdeg_prune)
    app.setPreprune([=](const auto g) {

        int target_nv = maxnv(g);
        int current_nv = num_vertices(g);
        int levs = target_nv - current_nv; 
        
        int excess = 0;
        int d3 = 0;
        int d4 = 0;
        int d3a = -1, d3b = -1;

        for (int i = 0; i < current_nv; ++i)
        {
            int d = g.degree[i]; 

            if (d == 3) {
                ++d3;
                d3a = d3b;
                d3b = i;
            }
            else if (d == 4) {
                ++d4;
            }
            else if (d > MAX_DEG) {
                excess += d - MAX_DEG;
            }
        }

        
        if (excess == 0) return KEEP; 

        if (d3 > 2) return PRUNE; 

        if (d3 == 2 && !commonedge(d3a, d3b, g)) return PRUNE; 

        if (d3 > 0 && excess >= levs) return PRUNE; 

        int i_calc = d3 + d3 + d4;
        if (i_calc > 0 && excess > levs - i_calc + 2) return PRUNE;

        return KEEP; 
    });


    // 2. PRUNE (Final filter)
    app.setPrune([=](const App::GraphView& g) {
        
        int nv = num_vertices(g);
        for (int i = 0; i < nv; ++i) {
            // Check if any vertex exceeds the maximum allowed degree
            if (out_degree(i, g) > MAX_DEG) {
                return PRUNE; 
            }
        }
        return KEEP;
    });

    return app.run();
}



//  Testing results MAXDEG7 a pocet vrcholu


/* PERFORMANCE BENCHMARK
 * Comparison between native './plantri_maxd -D7 14' (C implementation) and this C++ wrapper.
 *
 * Vertices (n) | Native Plantri (s) | C++ Wrapper (s)
 * -------------|--------------------|----------------
 * 16           |        1.45        |       1.41
 * 17           |        5.13        |       5.23
 * 18           |       19.07        |      19.53
 */
