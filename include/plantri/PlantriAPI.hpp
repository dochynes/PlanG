#pragma once
#include <functional>
#include <cstdio>
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

// firstedge[v] -> EDGE*
EDGE** pt_firstedge_array();  // //ukazatel na pole na pole firstedge[0 ... nv-1]
int pt_maxnv();

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
    Sprase6, //-s
    Ascii, // -a
    EdgeCode, //-E
    DoubleCode, //-T
    None //u

};


struct Backend 
{

    using GraphView = plantri::GraphView;


    inline static int n = 0;
    inline static bool dual = false;

    inline static GraphClass graph_class = GraphClass::Trinagulation;

    inline static int param_min_deg = -1; // -m#
    inline static int param_conn = -1; // -c#
    inline static bool param_exact_conn = false; // -x
    inline static int param_disk_outer = -1;   // -P#   velikost vnejsi steny
    inline static int param_max_face = -1;  //-f#

    inline static OutputFormat param_format = OutputFormat::PlanarCode;
    inline static bool param_header = false;   // -h
    inline static bool param_output_dual = false; // -d prepinac
    inline static bool param_one_iso_class= false; // -o  one member of each isomorphism class is written
    inline static bool param_group = false;       // -G  ensures that the full automorphism group is computed for each output graph, for PRUNE!!!
    inline static bool param_nontriv_group = false;  // -V   Only output graphs with non-trivial group


    inline static int param_res = -1;
    inline static int param_mod = -1;
    inline static int param_split_level = 0;  // -X , -XX...

    static void setVertices(int nn) 
    {
        n = nn; 
    }

    static void setDualCountMode(bool enabled = true)
    {
        dual = enabled;
    }

    static void setClass(GraphClass c)
    {
        graph_class =c;
    }

    static void setDiskSize(int outer_size)
    {
        graph_class=GraphClass::Disk;
        param_disk_outer = outer_size;
    }

    static void setMinDegree(int m)
    {
        param_min_deg = m;
    }
    
    static void setConnectivity(int c, bool exact = false)
    {
        param_conn = c;
        param_exact_conn = exact;
    }

    static void setMaxFaceSize(int f)
    {
        param_max_face=f;
    }

    static void setFormat(OutputFormat f)
    {
        param_format = f;
    }

    static void setNoOutput()
    {
        param_format = OutputFormat::None;
    }

    static void setOutputDual(bool output_dual = true)
    {
        param_output_dual = output_dual;
    }

    static void setHeader(bool h = true)
    {
        param_header=h;
    }

    static void setOrientationPreserving(bool o = true)
    {
        param_one_iso_class = o;
    }

    static void setFullGroup(bool g = true)
    {
        param_group = g;
    }

    static void setNonTrivialGroup(bool v = true)
    {
        param_nontriv_group = v;
    }

    static void setDistribution(int res, int mod)
    {
        param_res = res;
        param_mod = mod;
    }

    static void setSplitLevel(int level)
    {
        param_split_level = level;
    }


    //TODO add params processor








    static void setPrune(std::function<int(const GraphView&)> f)
    {
        plantri::setFilter(f);
    }

    static void setPreprune(std::function<int(const GraphView&)> f)
    {
        plantri::setPrefilter(f);
    }

    static void setOutproc(std::function<void(Output&, const GraphView&)> f) 
    { 
        plantri::setOutproc(f); 
    }

    static int run(int argc, char** argv) 
    { 

        return plantri::pt_run(argc, argv); 
    }


};


inline Output& operator<<(Output& out, const GraphView& g)
{
    if (!out.raw()) 
    {
        return out; 
    }

    ::pt_write_planar_code(out.raw(),0);

    return out;
}

}