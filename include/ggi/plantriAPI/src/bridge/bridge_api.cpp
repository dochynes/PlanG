#include "plantri/BridgeAPI.hpp"
#include <utility>
#include <functional>

extern "C" {

void  pt_set_prefilter(int (*f)(void));
void  pt_set_filter(int (*f)(int,int,int));
int   pt_run(int argc, char** argv);

int   pt_nv(void);
int   pt_ne_oriented(void);
int*  pt_degree_array(void);
int   pt_missing_vertex(void);

EDGE** pt_firstedge_array(void);

int pt_maxnv(void);
}


namespace {
    std::function<int()> g_cpp_prefilter;
    std::function<int(int,int,int)> g_cpp_filter;

    int c_prefilter_trampoline() 
    {
        try { 
            return g_cpp_prefilter ? g_cpp_prefilter() : 1; 
        }
        catch(...) 
        { 
            return 0; 
        }
    }
    int c_filter_trampoline(int nbtot, int nbop, int doflip) {
        try {
             return g_cpp_filter ? g_cpp_filter(nbtot, nbop, doflip) : 0; 
            }
        catch(...) 
        {
            return 0; 
        }
    }
}


namespace plantri {

void setPrefilter(std::function<int()> f) {
    g_cpp_prefilter = std::move(f);
    ::pt_set_prefilter(&c_prefilter_trampoline);
}

void setFilter(std::function<int(int,int,int)> f) {
    g_cpp_filter = std::move(f);
    ::pt_set_filter(&c_filter_trampoline);
}

int pt_run(int argc, char** argv) {
    return ::pt_run(argc, argv);
}

int pt_nv()
{ 
    return ::pt_nv(); 
}
int pt_ne_oriented()
{ 
    return ::pt_ne_oriented(); 
}
int* pt_degree_array()
{
    return ::pt_degree_array(); 
}
int pt_missing_vertex()
{ 
    return ::pt_missing_vertex(); 
}

EDGE** pt_firstedge_array() 
{ 
    return ::pt_firstedge_array(); 
}

int pt_maxnv() 
{ 
    return ::pt_maxnv(); 
}

} 