#pragma once
#include <functional>
#include <cstdio>
#include "gtools.h"
#include "common/Output.hpp"
#include "geng/GraphView.hpp"
#include <vector>
#include <string>
#include <utility>
namespace geng {


struct GraphView;

inline Output& operator<<(Output& out, const GraphView& g)
{
    if(out.raw())
        ::writeg6(out.raw(),const_cast<graph*>(g.data()),1,g.num_vertices());
    
    return out;
}

using OutprocFn = std::function<void(Output& out, const GraphView& g)>;
using PruneFn = std::function<int (const GraphView& g)>;
using PrepruneFn = std::function<int (const GraphView& g)>;

// registrace callbacku
void setOutproc(OutprocFn);
void setPrune(PruneFn);
void setPreprune(PrepruneFn); 
void setColors(int count);
void setColorClassSizes(const std::vector<int>& sizes);
void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds);
int distance_between(const GraphView& g, int src, int dst);


int run(int argc, char** argv);

// globaly/flagy
int mindeg();
int maxdeg();
int mine();
int maxe();
int connec();

bool flagSparse6();
bool flagGraph6();
bool flagQuiet();
bool flagNoOutput();
bool flagNautyFormat();
bool flagCanonise();


struct Backend
{

    using GraphView = geng::GraphView;


    enum class OutputFormat
    {
        Graph6, //default nebo -g
        Sprase6, //-s
        NoOutput, //-u
        NautyBinary // -n 
    };

    inline static int n = 1;
    inline static int mine = -1; //min edges
    inline static int maxe = -1; //max edges

    inline static int param_res = -1;
    inline static int param_mod = -1;

    // [-cCmtfkbd#D#]
    inline static bool param_connected = false; //-c
    inline static bool param_biconnected = false; //-C
    inline static bool param_save_mem = false; // -m
    inline static bool param_triangle_free=false; //-t
    inline static bool param_square_free = false; //-f
    inline static bool param_regular = false; // -k
    inline static bool param_bipartite = false; //-b

    inline static int param_min_deg = -1;  // -d
    inline static int param_max_deg = -1;  // -D

    //[-kTSPF]
    inline static bool param_chordal = false; // -T
    inline static bool param_split = false; // -S
    inline static bool param_perfect = false; // -P
    inline static bool param_claw_free = false; //-F



    //[-uygsnh]
    inline static OutputFormat out_format = OutputFormat::Graph6;
    inline static bool param_header = false; // -h

    //[-lvq]
    inline static bool param_label = false; // -l
    inline static bool param_verbose = false; //-v
    inline static bool param_quiet = false; //-q
    
    //[-x#X#]
    inline static int param_adv_split = -1; // -x
    inline static int param_adv_start = -1; // -X

    inline static std::string param_out_file = "";


    static void setVertices(int nn)
    {
        n=nn;
    }

    static void setEdgeRange(int min_edges, int max_edges = -1)
    {
        mine = min_edges;
        maxe = max_edges;
    }

    static void setConnected(bool c = true)
    {
        param_connected = c;
    }
    static void setBiconnected(bool b = true)
    {
        param_biconnected = b;
    }

    static void setTriangleFree(bool t = true)
    {
        param_triangle_free = t;
    }

    static void setSquareFree(bool s = true)
    {
        param_square_free = s;
    }

    static void setBipartite(bool b = true)
    {
        param_bipartite = b;

    }

    static void setMinDegree(int d)
    {
        param_min_deg = d;
    }

    static void setMaxDegree(int D)
    {
        param_max_deg = D;
    }

    static void setFormat(OutputFormat f)
    {
        out_format = f;
    }

    static void setNoOutput()
    {
        out_format = OutputFormat::NoOutput;
    }

    static void setCanonicalLabeling(bool l = true)
    {
        param_label = l;
    }

    static void setHeader (bool h=true)
    {
        param_header = h;
    }

    static void setQuiet(bool q=true)
    {
        param_quiet=q;
    }

    static void setVerbose( bool v =true)
    {
        param_verbose = v;
    }

    static void setSaveMemory (bool m = true)
    {
        param_save_mem = m;
    }

    static void setDistribution(int res, int mod)
    {
        param_res = res;
        param_mod = mod;
    }

    static void setAdvancedSplit(int x)
    {
        param_adv_split = x;
    }

    static void setAdvancedStartLevel(int X)
    {
        param_adv_start = X;
    }

    static void setRegular(bool r = true)
    {
        param_regular = r;
    }

    static void setChordal(bool enable = true) 
    {
        param_chordal = enable;
    }

    
    static void setSplit(bool enable = true) 
    {
        param_split = enable;
    }

    
    static void setPerfect(bool enable = true) 
    {
        param_perfect = enable;
    }

    
    static void setClawFree(bool enable = true) 
    {
        param_claw_free = enable;
    }

    static void setOutputFile(const std::string& filename)
    {
        param_out_file = filename;
    }

    static std::vector<std::string> prepare_args()
    {
        std::vector<std::string> args;

        args.push_back("geng");

        if(param_connected)
            args.push_back("-c");
        if(param_biconnected)
            args.push_back("-C");
        if(param_triangle_free)
            args.push_back("-t");
        if(param_square_free)
            args.push_back("-f");
        if(param_bipartite)
            args.push_back("-b");
        if(param_save_mem)
            args.push_back("-m");
        if(param_regular)
            args.push_back("-k");

        if(param_min_deg>=0)
        {
            args.push_back("-d" + std::to_string(param_min_deg));
        }
        if(param_max_deg>=0)
        {
            args.push_back("-D" + std::to_string(param_max_deg));
        }

        if(param_chordal)
            args.push_back("-T");
        if(param_split)
            args.push_back("-S");
        if(param_perfect)
            args.push_back("-P");
        if(param_claw_free)
            args.push_back("-F");


        // -y chybi
        switch(out_format)
        {
            case OutputFormat::Sprase6:
                args.push_back("-s");
                break;
            case OutputFormat::NoOutput:
                args.push_back("-u");
                break;
            case OutputFormat::NautyBinary:
                args.push_back("-n");
                break;
            case OutputFormat::Graph6:
                break;

        }


        if(param_header)
            args.push_back("-h");


        if(param_label)
            args.push_back("-l");
        if(param_verbose)
            args.push_back("-v");
        if(param_quiet)
            args.push_back("-q");


        
        if(param_adv_split >= 0)
        {
            args.push_back("-x" + std::to_string(param_adv_split));
        }
        if(param_adv_start>=0)
        {
            args.push_back("-X" + std::to_string(param_adv_start));
        }



        args.push_back(std::to_string(n));

        if(mine >= 0)
        {
            std::string edge_arg = std::to_string(mine);
            if(maxe>=0)
            {
                edge_arg += ":" + std::to_string(maxe);
            }
            args.push_back(edge_arg);
        }


        if(param_res>=0 && param_mod > 0)
        {
            args.push_back(std::to_string(param_res) + "/" + std::to_string(param_mod));
        }

        if(param_out_file != "")
        {
            args.push_back(param_out_file);
        }


        return args;


    }





    
    static void setPrune(std::function<int(const GraphView)> f )
    {
        geng::setPrune(f);
    }

    static void setPreprune(std::function<int(const GraphView&)> f)
    {
        geng::setPreprune(f);
    }

    static void setOutproc(std::function<void(Output&,const GraphView&)> f)
    {
        geng::setOutproc(f);
    }

    static int run(int argc, char** argv)
    { 
        return geng::run(argc, argv); 
    }

    static void setRootedVertices(int count)
    {
        std::vector<int> sizes;
        const int rooted_count = std::max(0, count);

        sizes.reserve(static_cast<std::size_t>(rooted_count) + 1);
        sizes.push_back(std::max(0, n - rooted_count));
        for (int i = 0; i < rooted_count; ++i)
            sizes.push_back(1);

        geng::setColorClassSizes(sizes);
    }

    static void setColors(int count)
    {
        geng::setColors(count);
    }

    static void setColorClassSizes(const std::vector<int>& sizes)
    {
        geng::setColorClassSizes(sizes);
    }

    static void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds)
    {
        geng::setColorClassBounds(bounds);
    }

    static int distance_between(const GraphView& g, int src, int dst)
    {
        return geng::distance_between(g, src, dst);
    }
};





} 
