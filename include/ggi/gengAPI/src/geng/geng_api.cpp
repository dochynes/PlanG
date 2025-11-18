
#include "geng/GengAPI.hpp"
#include "geng/GraphView.hpp"
#include <utility>

extern "C" 
{
void geng_set_outproc(void(*)(FILE*,void*,int));
void geng_set_prune(int(*)(void*,int,int));
void geng_set_preprune(int(*)(void*,int,int));
int  geng_run(int argc, char** argv);

int  geng_get_mindeg(void);
int  geng_get_maxdeg(void);
int  geng_get_mine(void);
int  geng_get_maxe(void);
int  geng_get_connec(void);

int  geng_flag_sparse6(void);
int  geng_flag_graph6(void);
int  geng_flag_quiet(void);
int  geng_flag_nooutput(void);
int  geng_flag_nautyformat(void);
int  geng_flag_canonise(void);
}

namespace 
{/*
    std::function<void(FILE*,const void*,int)> g_outproc;
    std::function<int(const void*,int,int)> g_prune;
    std::function<int(const void*,int,int)> g_preprune;

    void c_outproc(FILE* f, void* g, int n)
    {
        try { 
            if (g_outproc) g_outproc(f, g, n); 
            } 
        catch(...) {}
    }
    int c_prune(void* g, int n, int maxn)
    {
        try{ 
            return g_prune ? g_prune(g, n, maxn) : 0; 
            } 
        catch(...) { return 1; }
    }
    int c_preprune(void* g, int n, int maxn)
    {
        try{ 
            return g_preprune ? g_preprune(g, n, maxn) : 0; 
           } 
        catch(...) { return 1; }
    }
        */
}

namespace geng {

static OutprocFn s_outproc;
static PruneFn s_prune;
static PrepruneFn s_preprune;

static void c_outproc(FILE* f, void* gg, int n)
{
    if (!s_outproc) return;

    GraphView view
    {
        static_cast<const graph*>(gg),
        n,
        n
    };

    s_outproc(f, view);
}

static int c_preprune(void* gg, int n, int maxn)
{
    if(!s_preprune)
        return 0;

    GraphView view 
    {
        static_cast<const graph*>(gg),
        n,
        maxn
    };

    return s_preprune(view);
}

static int c_prune(void* gg, int n, int maxn)
{
    if (!s_prune) return 0;

    GraphView view
    {
        static_cast<const graph*>(gg),
        n,
        maxn
    };

    return s_prune(view);
}

void setOutproc(OutprocFn f)
{
    s_outproc = std::move(f);
    ::geng_set_outproc(&c_outproc);
}
void setPrune(PruneFn f)
{
    s_prune = std::move(f);
    ::geng_set_prune(&c_prune);
}
void setPreprune(PrepruneFn f)
{
    s_preprune = std::move(f);
    ::geng_set_preprune(&c_preprune);
}

int run(int argc, char** argv){ return ::geng_run(argc, argv); }

int mindeg()
{
    return ::geng_get_mindeg(); 
}
int maxdeg()
{ 
    return ::geng_get_maxdeg(); 
}
int mine()
{ 
    return ::geng_get_mine(); 
}
int maxe()
{ 
    return ::geng_get_maxe(); 
}
int connec()
{ 
    return ::geng_get_connec(); 
}

bool flagSparse6()
{    
    return ::geng_flag_sparse6()!=0; 
}
bool flagGraph6()
{   
    return ::geng_flag_graph6()!=0; 
}
bool flagQuiet()
{    
    return ::geng_flag_quiet()!=0; 
}
bool flagNoOutput()
{   
    return ::geng_flag_nooutput()!=0; 
}
bool flagNautyFormat()
{
    return ::geng_flag_nautyformat()!=0; 
}
bool flagCanonise()
{   
    return ::geng_flag_canonise()!=0; 
}

} 