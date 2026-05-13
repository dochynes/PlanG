#include "plantri/PlantriAPI.hpp"
#include "common/Output.hpp"
#include <utility>
#include <functional>
#include <iostream>

extern "C" {

void  pt_set_prefilter(int (*f)(void));
void  pt_set_filter(int (*f)(int,int,int));
int   pt_run(int argc, char** argv);
void  pt_write_current_graph(FILE* f, int doflip);

int   pt_nv(void);
int   pt_ne_oriented(void);
int*  pt_degree_array(void);
int   pt_missing_vertex(void);

EDGE** pt_firstedge_array(void);

int pt_maxnv(void);

FILE* pt_outfile(void);

void disable_summary(void);
void enable_summary(void);


}


namespace {

    plantri::PrefilterFn g_cpp_prefilter;
    plantri::FilterFn g_cpp_filter;
    plantri::OutprocFn g_cpp_outproc;
    int g_current_doflip = 0;
    inline plantri::GraphView make_view()
    {
        plantri::GraphView v{
            ::pt_firstedge_array(),
            ::pt_degree_array(),
            ::pt_nv(),
            ::pt_ne_oriented(),
            ::pt_missing_vertex(),
            ::pt_maxnv()
        };
        return v;
    }

    //std::function<int()> g_cpp_prefilter;
    //std::function<int(int,int,int)> g_cpp_filter;

    int c_prefilter_trampoline() 
    {


         try { 
            if(!g_cpp_prefilter) // --- 
            {
                return 1;
            }
            auto view = make_view();
            return g_cpp_prefilter(view);
        }
        catch(...) 
        { 
            return 0;  
        }
       
    }


    int c_filter_trampoline(int nbtot, int nbop, int doflip) 
    {

        //std::cout << "trampoline";

        (void)nbtot;
        (void)nbop;
        try
        {
            auto view = make_view();
            int should_prune = 0; //  1

            if (g_cpp_filter)
            {
                
                should_prune = g_cpp_filter(view);
            }
            

            if (should_prune == 1) 
            {
                return 1; 
            }

            if (g_cpp_outproc)
            {
                
                FILE* f = ::pt_outfile();
                Output out(f);
                g_current_doflip = doflip;
                g_cpp_outproc(out, view);
                g_current_doflip = 0;
                
               
                return 1;
            }

            return 0;

        }
        catch(...) 
        {
            g_current_doflip = 0;
            return 1; 
        }


        
    }

    void write_current_graph(FILE* f)
    {
        ::pt_write_current_graph(f, g_current_doflip);
    }

    void refresh_filter_registration()
    {
        if (g_cpp_filter || g_cpp_outproc)
            ::pt_set_filter(&c_filter_trampoline);
        else
            ::pt_set_filter(nullptr);
    }
}


namespace plantri {

void setPrefilter(PrefilterFn f) 
{
    g_cpp_prefilter = std::move(f);
    if (g_cpp_prefilter)
        ::pt_set_prefilter(&c_prefilter_trampoline);
    else
        ::pt_set_prefilter(nullptr);
}

void setFilter(FilterFn f) 
{
    g_cpp_filter = std::move(f);
    refresh_filter_registration();
}

void setOutproc(OutprocFn f)
{
    g_cpp_outproc = std::move(f);
    refresh_filter_registration();

    if (g_cpp_outproc)
        ::disable_summary();
    else
        ::enable_summary();
}

Output& operator<<(Output& out, const GraphView& g)
{
    (void)g;

    if (!out.raw())
    {
        return out;
    }

    write_current_graph(out.raw());

    return out;
}

int pt_run(int argc, char** argv) 
{
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
