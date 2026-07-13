#pragma once
#include <functional>
#include <cstdio>
#include "gtools.h"
#include "common/Output.hpp"
#include "geng/GraphView.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <string>
#include <utility>
namespace geng {


struct GraphView;

// umoznuje v callbacku zapsat graf pomoci out << g
inline Output& operator<<(Output& out, const GraphView& g)
{
    if(out.raw())
        ::writeg6(out.raw(),const_cast<graph*>(g.data()),1,g.num_vertices());
    
    return out;
}

// callbacky pouzivane backendem geng
using OutprocFn = std::function<void(Output& out, const GraphView& g)>;
using FilterFn = std::function<int (const GraphView& g)>;
using PrepruneFn = std::function<int (const GraphView& g)>;

// funkce volane backendem pri priprave behu generatoru
void setOutproc(OutprocFn);
void setFilter(FilterFn);
void setPreprune(PrepruneFn); 
void setColors(int count);
void setColorCount(int count);
void setColorClassSizes(const std::vector<int>& sizes);
void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds);
int distance_between(const GraphView& g, int src, int dst);


int run(int argc, char** argv);

// Konfigurace backendu geng. Objekt uklada volby uzivatele, prevadi je na
// argumenty puvodniho programu geng a pred spustenim nastavi callbacky/barvy.
// Bezne se pouziva pres Generator<geng::Backend>.
class Backend
{
public:

    using GraphView = geng::GraphView;


    enum class OutputFormat
    {
        Graph6,      // vychozi vystup, odpovida -g
        Sparse6,     // -s
        NoOutput,    // -u
        NautyBinary  // -n
    };

private:
    // zpusob prace s barvami vrcholu.
    enum class ColorMode
    {
        None,
        Count,
        Sizes,
        Bounds
    };

    int n = 1;
    int mine = -1; //min edges
    int maxe = -1; //max edges

    int param_res = -1;
    int param_mod = -1;

    // [-cCmtfkbd#D#]
    bool param_connected = false; //-c
    bool param_biconnected = false; //-C
    bool param_save_mem = false; // -m
    bool param_triangle_free=false; //-t
    bool param_square_free = false; //-f
    bool param_regular = false; // -k
    bool param_bipartite = false; //-b

    int param_min_deg = -1;  // -d
    int param_max_deg = -1;  // -D

    //[-kTSPF]
    bool param_chordal = false; // -T
    bool param_split = false; // -S
    bool param_perfect = false; // -P
    bool param_claw_free = false; //-F



    //[-uygsnh]
    OutputFormat out_format = OutputFormat::Graph6;
    bool param_header = false; // -h

    //[-lvq]
    bool param_label = false; // -l
    bool param_verbose = false; //-v
    bool param_quiet = false; //-q
    
    //[-x#X#]
    int param_adv_split = -1; // -x
    int param_adv_start = -1; // -X

    std::string param_out_file = "";

    ColorMode color_mode = ColorMode::None;
    int color_count = 0;
    std::vector<int> color_sizes;
    std::vector<std::pair<int,int>> color_bounds;

    FilterFn filter;
    PrepruneFn preprune;
    OutprocFn outproc;

    static constexpr int max_vertices = 64;

    static int max_possible_edges(int vertices)
    {
        return (vertices * (vertices - 1)) / 2;
    }

public:
    
    void setVertices(int nn)
    {
        if(nn < 1 || nn > max_vertices)
            throw std::invalid_argument("geng::Backend::setVertices expects n in range 1.." + std::to_string(max_vertices));
        n=nn;
    }

    void setEdgeRange(int min_edges, int max_edges = -1)
    {
        if(min_edges < 0)
            throw std::invalid_argument("geng::Backend::setEdgeRange expects min_edges >= 0");
        if(max_edges < -1)
            throw std::invalid_argument("geng::Backend::setEdgeRange expects max_edges >= min_edges or -1");
        if(max_edges >= 0 && max_edges < min_edges)
            throw std::invalid_argument("geng::Backend::setEdgeRange expects max_edges >= min_edges");

        mine = min_edges;
        maxe = max_edges;
    }

    void setConnected(bool c = true)
    {
        param_connected = c;
    }
    void setBiconnected(bool b = true)
    {
        param_biconnected = b;
    }

    void setTriangleFree(bool t = true)
    {
        param_triangle_free = t;
    }

    void setSquareFree(bool s = true)
    {
        param_square_free = s;
    }

    void setBipartite(bool b = true)
    {
        param_bipartite = b;

    }

    void setMinDegree(int d)
    {
        if(d < 0 || d >= max_vertices)
            throw std::invalid_argument("geng::Backend::setMinDegree expects d in range 0.." + std::to_string(max_vertices - 1));
        param_min_deg = d;
    }

    void setMaxDegree(int D)
    {
        if(D < 0 || D >= max_vertices)
            throw std::invalid_argument("geng::Backend::setMaxDegree expects D in range 0.." + std::to_string(max_vertices - 1));
        param_max_deg = D;
    }

    void setFormat(OutputFormat f)
    {
        out_format = f;
    }

    void setNoOutput()
    {
        out_format = OutputFormat::NoOutput;
    }

    void setCanonicalLabeling(bool l = true)
    {
        param_label = l;
    }

    void setHeader (bool h=true)
    {
        param_header = h;
    }

    void setQuiet(bool q=true)
    {
        param_quiet=q;
    }

    void setVerbose( bool v =true)
    {
        param_verbose = v;
    }

    void setSaveMemory (bool m = true)
    {
        param_save_mem = m;
    }

    // res/mod.
    void setDistribution(int res, int mod)
    {
        if(mod <= 0)
            throw std::invalid_argument("geng::Backend::setDistribution expects mod > 0");
        if(res < 0 || res >= mod)
            throw std::invalid_argument("geng::Backend::setDistribution expects 0 <= res < mod");

        param_res = res;
        param_mod = mod;
    }

    void setAdvancedSplit(int x)
    {
        if(x < 0)
            throw std::invalid_argument("geng::Backend::setAdvancedSplit expects x >= 0");
        param_adv_split = x;
    }

    void setAdvancedStartLevel(int X)
    {
        if(X < 0)
            throw std::invalid_argument("geng::Backend::setAdvancedStartLevel expects X >= 0");
        param_adv_start = X;
    }

    void setRegular(bool r = true)
    {
        param_regular = r;
    }

    void setChordal(bool enable = true) 
    {
        param_chordal = enable;
    }

    
    void setSplit(bool enable = true) 
    {
        param_split = enable;
    }

    
    void setPerfect(bool enable = true) 
    {
        param_perfect = enable;
    }

    
    void setClawFree(bool enable = true) 
    {
        param_claw_free = enable;
    }

    void setOutputFile(const std::string& filename)
    {
        param_out_file = filename;
    }

protected:
    // Sestavi argv pro puvodni geng podle ulozene konfigurace
    std::vector<std::string> prepare_args() const
    {
        validate();

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


        // Stary y-format puvodniho geng neni podporovan.
        switch(out_format)
        {
            case OutputFormat::Sparse6:
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






public:
    
    void setFilter(FilterFn f)
    {
        filter = std::move(f);
    }

    void setPreprune(PrepruneFn f)
    {
        preprune = std::move(f);
    }

    void setOutproc(OutprocFn f)
    {
        outproc = std::move(f);
    }

protected:
    static int run(int argc, char** argv)
    { 
        return geng::run(argc, argv); 
    }

public:
    
    void setRootedVertices(int count)
    {
        if(count < 0)
            throw std::invalid_argument("geng::Backend::setRootedVertices expects count >= 0");
        if(count > n)
            throw std::invalid_argument("geng::Backend::setRootedVertices expects count <= number of vertices");

        std::vector<int> sizes;
        const int rooted_count = count;

        sizes.reserve(static_cast<std::size_t>(rooted_count) + (rooted_count < n ? 1 : 0));
        if (rooted_count < n)
            sizes.push_back(n - rooted_count);
        for (int i = 0; i < rooted_count; ++i)
            sizes.push_back(1);

        setColorClassSizes(sizes);
    }

    
    void setColors(int count)
    {
        if(count < 0)
            throw std::invalid_argument("geng::Backend::setColors expects count >= 0");

        color_mode = ColorMode::Count;
        color_count = count;
        color_sizes.clear();
        color_bounds.clear();
    }

    void setColorClassSizes(const std::vector<int>& sizes)
    {
        if(sizes.empty())
            throw std::invalid_argument("geng::Backend::setColorClassSizes expects at least one color class");
        for(int size : sizes)
        {
            if(size <= 0)
                throw std::invalid_argument("geng::Backend::setColorClassSizes expects positive class sizes");
        }

        color_mode = ColorMode::Sizes;
        color_sizes = sizes;
        color_bounds.clear();
        color_count = 0;
    }

    void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds)
    {
        if(bounds.empty())
            throw std::invalid_argument("geng::Backend::setColorClassBounds expects at least one color class");
        for(const auto& [lower, upper] : bounds)
        {
            if (lower < 0)
                throw std::invalid_argument("geng::Backend::setColorClassBounds expects lower bounds >= 0");
            if (upper < lower)
                throw std::invalid_argument("geng::Backend::setColorClassBounds expects upper >= lower");
        }

        color_mode = ColorMode::Bounds;
        color_bounds = bounds;
        color_sizes.clear();
        color_count = 0;
    }

protected:
    
    void apply_runtime_state() const
    {
        switch(color_mode)
        {
            case ColorMode::Count:
                geng::setColors(color_count);
                break;
            case ColorMode::Sizes:
                geng::setColorClassSizes(color_sizes);
                break;
            case ColorMode::Bounds:
                geng::setColorClassBounds(color_bounds);
                break;
            case ColorMode::None:
                geng::setColorCount(0);
                break;
        }

        geng::setFilter(filter);
        geng::setPreprune(preprune);
        geng::setOutproc(outproc);
    }

    void validate() const
    {
        if(n < 1 || n > max_vertices)
        {
            throw std::logic_error("geng configuration has invalid vertex count");
        }

        if(param_min_deg >= 0 && param_max_deg >= 0 && param_min_deg > param_max_deg)
        {
            throw std::logic_error("minimum degree cannot be greater than maximum degree");
        }


        if(mine >= 0)
        {
            const int edge_limit = max_possible_edges(n);
            if(mine > edge_limit || maxe > edge_limit)
                throw std::logic_error("too many edges requested for a graph");
        }

        if(param_res >= 0 && (param_mod <= 0 || param_res >= param_mod))
        {
            throw std::logic_error("geng configuration has invalid res/mod distribution");
        }

        switch(color_mode)
        {
            case ColorMode::Count:
                if (color_count > n)
                    throw std::logic_error("geng configuration has more required colors than vertices");
                break;
            case ColorMode::Sizes:
            {
                int total = std::accumulate(color_sizes.begin(), color_sizes.end(), 0);
                if(total != n)
                    throw std::logic_error("sum of color class sizes must equal the number of vertices");
                break;
            }
            case ColorMode::Bounds:
            {
                int min_total = 0;
                int max_total = 0;
                for (const auto& [lower, upper] : color_bounds)
                {
                    min_total += lower;
                    max_total += upper;
                }
                if (min_total > n || max_total < n)
                    throw std::logic_error("geng color class bounds cannot cover the configured number of vertices");
                break;
            }
            case ColorMode::None:
                break;
        }
    }

public:
    // Pomocna funkce pro vzdalenost dvou vrcholu v aktualnim GraphView
    static int distance_between(const GraphView& g, int src, int dst)
    {
        return geng::distance_between(g, src, dst);
    }
};





} 
