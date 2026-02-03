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


struct Backend 
{

    using GraphView = plantri::GraphView;


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