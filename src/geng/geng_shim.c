#include "gtools.h"


typedef void (*outproc_cb_t)(FILE* f, void* g, int n);
typedef int (*prune_cb_t)(void* g, int n, int maxn);

static outproc_cb_t g_outproc = NULL;
static prune_cb_t g_prune = NULL;
static prune_cb_t g_preprune = NULL;

extern int sparse6, graph6, quiet, nooutput, nautyformat, canonise;


void bridge_outproc(FILE* f, graph* g, int n) 
{
    if(g_outproc) 
    {
        g_outproc(f, (void*)g, n);
        return;
    }

    if(nautyformat)
    {
         return; 
    }
    if(nooutput)
    { 
        return;
    }
    else if (sparse6)
    { 
        writes6(f, g, 1, n); 
    }
    else 
    { 
        writeg6(f, g, 1, n); 
    }
}


int bridge_prune(graph* g, int n, int maxn) 
{
    return g_prune ? g_prune((void*)g, n, maxn) : 0;
}

int bridge_preprune(graph* g, int n, int maxn) 
{
    return g_preprune ? g_preprune((void*)g, n, maxn) : 0;
}


void geng_set_outproc(outproc_cb_t f)
{ 
    g_outproc = f; 
}
void geng_set_prune(prune_cb_t f)
{
     g_prune = f;
}
void geng_set_preprune(prune_cb_t f)
{
    g_preprune = f;
}


extern int geng_mindeg, geng_maxdeg, geng_mine, geng_maxe, geng_connec;

int geng_get_mindeg(void) 
{
    return geng_mindeg; 
}
int geng_get_maxdeg(void)
{
    return geng_maxdeg;
}
int geng_get_mine(void)
{
    return geng_mine;
}
int geng_get_maxe(void)
{
    return geng_maxe;
}
int geng_get_connec(void)
{
    return geng_connec;
}

int geng_flag_sparse6(void)
{
    return sparse6;
}
int geng_flag_graph6(void)
{
    return graph6;
}
int geng_flag_quiet(void)
{
    return quiet;
}
int geng_flag_nooutput(void)
{ 
    return nooutput; 
}
int geng_flag_nautyformat(void)
{ 
    return nautyformat;
}
int geng_flag_canonise(void)
{ 
    return canonise; 
}

int GENG_MAIN(int argc, char* argv[]); // vznikne pres -Dmain=GENG_MAIN
int geng_run(int argc, char** argv)
{ 
    return GENG_MAIN(argc, argv); 
}