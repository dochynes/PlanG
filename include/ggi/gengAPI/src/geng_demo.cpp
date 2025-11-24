#include "geng/GengAPI.hpp"

#include "geng/GraphView.hpp"
#include "geng/GraphViewBoost.hpp"
#include "geng/GraphViewFunctions.hpp"

#include <iostream>
#include <vector>


#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>  // identity_property_map, iterator_property_map
#include <boost/range/iterator_range.hpp>

/*
Dokumentace:
wrapper zpristupnuje generator geng(nauty) pomoci c++ API

generovani probiha stejne jako pri volani nativniho programu geng

knihovna umoznuje pridat vlastni logiku do generovani grafu 
- SetPreprune - levny filtr behem generovani 
- setPrune - filtr nad hotovym grafem 
- setOutproc - uzivatelske zpracovani vystupniho grafu

pokud uzivatel nenastavi vlastni outproc, pouzije se puvodni vystup (graph6)

vsechny callbacky dostavaji graf jako "const GraphView&" , (uzivatel nemusi znat presny typ, muze pouzit auto)
je to read-only zero-copy view na vnitrni bitovou representaci grafu v nauty.
GraphView implementuje nasledujici Boost.Graph koncepty: AdjacencyGraph,IncidenceGraph,VertexListGraph,EdgeListGraph (https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html)

diky tem konceptum nad vygenerovanym grafem muzeme volat funkce a algoritmy z knihovny boost :num_vertices(g), num_edges(g), 
vertices(g), edges(g), out_degree(v, g), adjacent_vertices(v, g),bfs,dfs ... 

graf je vzdy jednoduchy, neorientovany, vrcholy 0...n-1

PropertyGraph(https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html) jsem neimplementoval. 
Myslim ze by to zbytecne komplikovalo a slo proti zero-copy view
Misto toho, kdyz uzivatel potrebuje uchovat nejake vlastnosti(barvy,vhay...) by mel pouzit externi property mapy z boostu(jako jsou iterator_property_map, vector_property_map,associative_property_map ...)


*/


int main(int argc, char** argv){

    using namespace geng;
    using namespace boost;


    int n = 6;
    //parametry
    const int MAX_DEG = 4;
    const int MAX_COLORS = 3;

    geng::setPreprune([=](const auto& g) {

        //std::cout<<"preprune volani"<<std::endl;

        int maxdeg = 0;
        auto [first, last] = vertices(g);
        for (auto it = first;it != last;it++)
        {

                auto v = *it;
                maxdeg = std::max<int>(maxdeg, out_degree(v, g));
                if (maxdeg > MAX_DEG) 
                {
                        return 1;
                }

        }
        return 0;
    });


    geng::setPrune([](const auto& g){
        std::size_t nv = num_vertices(g);
        
        vector_property_map<std::size_t> color(nv);
        auto num_colors = boost::sequential_vertex_coloring(g, color);
        if(num_colors <= MAX_COLORS)
                return 0;
        return 1;

    });

    // 

    /*geng::setOutproc([=](FILE* f, const auto& g) 
    {

        std::fprintf(f," ggg  ");
    });*/ //TODO zabalit FILE* f  do nejake vlastni tridy Output

    std::string nstr = std::to_string(n);
    const char* args[] = {
        "geng",
        "-c",          
        nstr.c_str(),
        nullptr
    };

    int pargc = 0;
    while (args[pargc]) ++pargc;

    return geng::run(pargc, const_cast<char**>(args));


}



//geng dokumentace

//"geng [-cCmtfkbd#D#] [-kTSPF] [-uygsnh] [-lvq] \n\
              [-x#X#] n [mine[:maxe]] [res/mod] [file]"

/*  Parameters:

             n    = the number of vertices (1..MAXN)
                        Note that MAXN is limited to min(WORDSIZE,64)
             mine = the minimum number of edges (no bounds if missing)
             maxe = the maximum number of edges (same as mine if missing)
                    0 means "infinity" except in the case "0-0"
             mod, res = a way to restrict the output to a subset.
                        All the graphs in G(n,mine..maxe) are divided into
                        disjoint classes C(0,mod),C(1,mod),...,C(mod-1,mod),
                        of very approximately equal size.
                        Only the class C(res,mod) is written.

                        If the -x or -X switch is used, they must have the 
                        same value for different values of res; otherwise 
                        the partitioning may not be valid.  In this case
                        (-x,-X with constant value), the usual relationships 
                        between modulo classes are obeyed; for example 
                        C(3,4) = C(3,8) union C(7,8).  This is not true
                        if 3/8 and 7/8 are done with -x or -X values
                        different from those used for 3/4.

             file = a name for the output file (stdout if missing or "-")

             All switches can be concatenated or separate.  However, the
             value of -d must be attached to the "d", and similarly for "x".

             -c    : only write connected graphs
             -C    : only write biconnected graphs
             -t    : only generate triangle-free graphs
             -f    : only generate 4-cycle-free graphs
             -b    : only generate bipartite graphs
                        (-t, -f and -b can be used in any combination)
             -m    : save memory at expense of time (only makes a
                        difference in the absence of -b, -t, -f and n <= 30).
             -D<int> : specify an upper bound for the maximum degree.
                     The value of the upper bound must be adjacent to
                     the "D".  Example: -D6
             -d<int> : specify a lower bound for the minimum degree.
                     The value of the upper bound must be adjacent to
                     the "d".  Example: -d6
             -v    : display counts by number of edges
             -l    : canonically label output graphs

             -u    : do not output any graphs, just generate and count them
             -g    : use graph6 output (default)
             -s    : use sparse6 output
             -n    : use nauty format instead of graph6 format for output
             -y    : use the obsolete y-format for output
             -h    : for graph6 or sparse6 format, write a header too

             -q    : suppress auxiliary output (except from -v)

             -x<int> : specify a parameter that determines how evenly
                     the res/mod facility splits the graphs into subsets.
                     High values mean more even splitting at slight cost
                     to the total time.  The default is 20*mod, and the
                     the legal minimum is 3*mod.  More information is given 
                     under "res/mod" above.
             -X<lev> : move the initial splitting level higher by <lev>,
                     in order to force more even splitting at the cost
                     of speed.  Default is -X0.  More information is given
                     under "res/mod" above.

Output formats.

  The output format is determined by the mutually exclusive switches
  -u, -n, -y, -g and -s.  The default is -g.

  -u suppresses output of graphs completely.

  -s and -g specify sparse6 and graph6 format, defined elsewhere.
  In this case a header is also written if -h is present.

  If -y is present, graphs will be written in y-format.
  y-format is obsolete and only provided for backwards compatibility.

    Each graph occupies one line with a terminating newline.
    Except for the newline, each byte has the format  01xxxxxx, where
    each "x" represents one bit of data.
    First byte:  xxxxxx is the number of vertices n
    Other ceiling(n(n-1)/12) bytes:  These contain the upper triangle of
    the adjacency matrix in column major order.  That is, the entries
    appear in the order (0,1),(0,2),(1,2),(0,3),(1,3),(2,3),(0,4),... .
    The bits are used in left to right order within each byte.
    Any unused bits on the end are set to zero.

  If -n is present, any output graphs are written in nauty format.

    For a graph of n vertices, the output consists of one int giving
    the number of vertices, and n setwords containing the adjacency
    matrix.  Note that this is system dependent (i.e. don't use it).
    It will not work properly if the output is to stdout and your
    system distinguishes binary and text files.

    */