#pragma once
#include <functional>

extern "C" {


typedef struct EDGE /* The data type used for edges */
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


namespace plantri {
    // PRE-FILTER / FILTER registrace
void setPrefilter(std::function<int()> f);
void setFilter(std::function<int(int,int,int)> f);


int  pt_run(int argc, char** argv);


int   pt_nv();   // pocet vrcholu aktualniho grafu
int   pt_ne_oriented();  // pocet orientovanych hran (2xpocet hran) 
int*  pt_degree_array();  // ukazatel na interni pole stupnu vrcholu 
int   pt_missing_vertex();

// firstedge[v] -> EDGE*
EDGE** pt_firstedge_array();  // //ukazatel na pole na pole firstedge[0 ... nv-1]
}