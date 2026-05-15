#pragma once
#include <functional>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "common/Output.hpp"




extern "C" {


typedef struct e /* The data type used for edges */
{ 
    int start;         /* vertex where the edge starts */
    int end;           /* vertex where the edge ends */ 
    int rightface;     /* face on the right side of the edge
                          note: only valid if make_dual() called */
    struct e *prev;    /* previous edge in clockwise direction */
    struct e *next;    /* next edge in clockwise direction */
    struct e *invers;  /* the edge that is inverse to this one */
    struct e *min;     /* the least of e and e->invers */
    int mark,index,rf;    /* three ints for temporary use;
                          rf is only for the printing routines;
                          Only access mark via the MARK macros. */
    int left_facesize; /* size of the face in prev-direction of the edge.
                          Only used for -p option. */
} EDGE;

}

extern "C" {
    void pt_write_planar_code(FILE* f, int header);
    void pt_mute_status_output();
}



#include "plantri/GraphView.hpp"


namespace plantri {


using PrefilterFn = std::function<int(const GraphView&)>;
using FilterFn = std::function<int (const GraphView&)>;
using OutprocFn = std::function<void (Output& out, const GraphView&)>;
    // PRE-FILTER / FILTER registrace
//void setPrefilter(std::function<int()> f);
//void setFilter(std::function<int(int,int,int)> f);

void setPrefilter(PrefilterFn);
void setFilter(FilterFn);
void setOutproc(OutprocFn);


int pt_run(int argc, char** argv);


int pt_nv();   // pocet vrcholu aktualniho grafu
int pt_ne_oriented();  // pocet orientovanych hran (2xpocet hran) 
int* pt_degree_array();  // ukazatel na interni pole stupnu vrcholu 
int pt_missing_vertex();

// firstedge[v] -> EDGE*. pokud missing_vertex >= 0 platne jsou 0..nv krome missing_vertex
EDGE** pt_firstedge_array();
int pt_maxnv();




struct Backend 
{


    enum class GraphClass
    {
        Trinagulation, //dafault
        Quadrangulation, // -q
        GeneralQuad, // -Q
        SimplePlane, // -p
        Bipartite, // -bp  bipartite plane
        Eulerian, // -b 
        Disk, // -P   triangulations of a disk
        Apollonian //--A
    };

    enum class OutputFormat
    {
        PlanarCode, // default
        Graph6, // -g
        Sparse6, //-s
        Ascii, // -a
        EdgeCode, //-E
        DoubleCode, //-T
        NoOutput //u

    };


    using GraphView = plantri::GraphView;


    int n = 0;
    bool dual = false;

    GraphClass graph_class = GraphClass::Trinagulation;

    int param_min_deg = -1; // -m#
    int param_conn = -1; // -c#
    bool param_exact_conn = false; // -x
    int param_disk_outer = -1;   // -P#   velikost vnejsi steny
    int param_max_face = -1;  //-f#

    OutputFormat param_format = OutputFormat::PlanarCode;
    bool param_header = false;   // -h
    bool param_output_dual = false; // -d prepinac
    bool param_one_iso_class= false; // -o  one member of each isomorphism class is written
    bool param_group = false;       // -G  ensures that the full automorphism group is computed for each output graph, for PRUNE!!!
    bool param_nontriv_group = false;  // -V   Only output graphs with non-trivial group


    int param_res = -1;
    int param_mod = -1;
    int param_split_level = 0;  // -X , -XX...

    std::string param_out_file = "";

    PrefilterFn prefilter;
    FilterFn filter;
    OutprocFn outproc;

    static constexpr int max_vertices = 64;

    void setOutputFile(std::string outFile)
    {
        param_out_file = outFile;
    }

    void setVertices(int nn) 
    {
        if(nn < 1 || nn > max_vertices)
            throw std::invalid_argument("plantri::Backend::setVertices expects n in range 1.." + std::to_string(max_vertices));
        n = nn; 
    }

    void setDualCountMode(bool enabled = true)
    {
        dual = enabled;
    }

    void setClass(GraphClass c)
    {
        graph_class =c;
    }

    void setDiskSize(int outer_size)
    {
        if(outer_size < 3)
            throw std::invalid_argument("plantri::Backend::setDiskSize expects outer_size >= 3");
        graph_class=GraphClass::Disk;
        param_disk_outer = outer_size;
    }

    void setMinDegree(int m)
    {
        if(m < 1 || m > 5)
            throw std::invalid_argument("plantri::Backend::setMinDegree expects m in range 1..5");
        param_min_deg = m;
    }
    
    void setConnectivity(int c, bool exact = false)
    {
        if(c < 1 || c > 5)
            throw std::invalid_argument("plantri::Backend::setConnectivity expects c in range 1..5");
        param_conn = c;
        param_exact_conn = exact;
    }

    void setMaxFaceSize(int f)
    {
        if(f < 3)
            throw std::invalid_argument("plantri::Backend::setMaxFaceSize expects f >= 3");
        param_max_face=f;
    }

    void setFormat(OutputFormat f)
    {
        param_format = f;
    }

    void setNoOutput()
    {
        param_format = OutputFormat::NoOutput;
    }

    void setOutputDual(bool output_dual = true)
    {
        param_output_dual = output_dual;
    }

    void setHeader(bool h = true)
    {
        param_header=h;
    }

    void setOrientationPreserving(bool o = true)
    {
        param_one_iso_class = o;
    }

    void setFullGroup(bool g = true)
    {
        param_group = g;
    }

    void setNonTrivialGroup(bool v = true)
    {
        param_nontriv_group = v;
    }

    void setDistribution(int res, int mod)
    {
        if(mod <= 0)
            throw std::invalid_argument("plantri::Backend::setDistribution expects mod > 0");
        if(res < 0 || res >= mod)
            throw std::invalid_argument("plantri::Backend::setDistribution expects 0 <= res < mod");

        param_res = res;
        param_mod = mod;
    }

    void setSplitLevel(int level)
    {
        if(level < 0)
            throw std::invalid_argument("plantri::Backend::setSplitLevel expects level >= 0");
        param_split_level = level;
    }


    //TODO add params processor
    std::vector<std::string> prepare_args() const
    {
        validate();

        std::vector<std::string> args;
        args.push_back("plantri");

        switch(graph_class)
        {
            case GraphClass::Quadrangulation:
                args.push_back("-q");
                break;
            case GraphClass::GeneralQuad:
                args.push_back("-Q");
                break;
            case GraphClass::SimplePlane:
                args.push_back("-p");
                break;
            case GraphClass::Bipartite:
                args.push_back("-bp");
                break;
            case GraphClass::Eulerian:
                args.push_back("-b");
                break;
            case GraphClass::Apollonian:
                args.push_back("-A");
                break;
            case GraphClass::Disk:
                if(param_disk_outer > 0)
                {
                    std::string flag = "-P" + std::to_string(param_disk_outer);
                    args.push_back(flag);
                }
                else
                {
                    args.push_back("-P");
                }
                break;
            case GraphClass::Trinagulation:
                break;
       }

       if(param_min_deg>0)
       {
            args.push_back("-m" + std::to_string(param_min_deg));
       }

       if(param_conn>=0)
       {
            std::string flag = "-c" + std::to_string(param_conn);
            if(param_exact_conn)
                flag += "x";
            args.push_back(flag);
       }

       if(param_max_face>=0)
       {
            args.push_back("-f" + std::to_string(param_max_face));
       }

       switch(param_format)
       {
            case OutputFormat::Graph6:
                args.push_back("-g");
                break;
            case OutputFormat::Sparse6:
                args.push_back("-s");
                break;
            case OutputFormat::Ascii:
                args.push_back("-a");
                break;
            case OutputFormat::EdgeCode:
                args.push_back("-E");
                break;
            case OutputFormat::DoubleCode:
                args.push_back("-T");
                break;
            case OutputFormat::NoOutput:
                args.push_back("-u");
                break;
            default: break;
       }

       if(param_header)
            args.push_back("-h");
       if(param_output_dual)
            args.push_back("-d");
        
       if(param_one_iso_class)
            args.push_back("-o");
       if(param_group)
            args.push_back("-G");
       if(param_nontriv_group)
            args.push_back("-V");
        

       //splitting
       if(param_split_level>0)
       {
            std::string x_flag = "-";
            for(int i =0 ;i<param_split_level;i++)
            {
                x_flag+="X";
            }
            args.push_back(x_flag);
       }


       std::string n_arg = std::to_string(n);
       if(dual)
            n_arg+="d";
       args.push_back(n_arg);

       if(param_res>=0 && param_mod>0)
            args.push_back(std::to_string(param_res) + "/" + std::to_string(param_mod));
       if(param_out_file != "")
            args.push_back(param_out_file);

        return args;

    }








    void setPrune(FilterFn f)
    {
        filter = std::move(f);
    }

    void setPreprune(PrefilterFn f)
    {
        prefilter = std::move(f);
    }

    void setOutproc(OutprocFn f) 
    { 
        outproc = std::move(f); 
    }

    static int run(int argc, char** argv) 
    { 

        return plantri::pt_run(argc, argv); 
    }

    void apply_runtime_state() const
    {
        plantri::setPrefilter(prefilter);
        plantri::setFilter(filter);
        plantri::setOutproc(outproc);
    }

    void validate() const
    {
        if(n < 1 || n > max_vertices)
        {
            throw std::logic_error("plantri configuration has invalid vertex count");
        }

        if(param_min_deg != -1 && (param_min_deg < 1 || param_min_deg > 5))
        {
            throw std::logic_error("plantri configuration has invalid minimum degree");
        }
        if(param_conn != -1 && (param_conn < 1 || param_conn > 5))
        {
            throw std::logic_error("plantri configuration has invalid connectivity");
        }
        if(param_max_face != -1 && param_max_face < 3)
        {
            throw std::logic_error("plantri configuration has invalid maximum face size");
        }
        if(param_res >= 0 && (param_mod <= 0 || param_res >= param_mod))
        {
            throw std::logic_error("plantri configuration has invalid res/mod distribution");
        }
    }


};


Output& operator<<(Output& out, const GraphView& g);

}
