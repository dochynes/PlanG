#include <stdio.h>
#include <stddef.h>
#include "nauty.h"    
#include "gtools.h"

typedef void (*outproc_cb_t)(FILE* f, void* g, int n); // 
typedef int (*prune_cb_t)(void* g, int n, int maxn);  // 0=> zahodit

static outproc_cb_t g_outproc = NULL;
static prune_cb_t   g_prune   = NULL;
static prune_cb_t   g_preprune= NULL;


// uzitecne flagy
extern int sparse6, graph6, quiet, nooutput, nautyformat, canonise;


void bridge_outproc(FILE* f, void* g, int n) 
{
    
    if (g_outproc) 
    {
        //printf("=> calling custom g_outproc\n");
        g_outproc(f, g, n);
        return;
    };

     if (nautyformat) 
     {
        //printf("=> TODO: nautyformat not implemented, skipping\n");
        //
    }

    if(nooutput)
    {
        //printf("=> nooutput=TRUE, skipping output\n");
        return;
    }
        
    
    else if (sparse6)
    {
        
        writes6(f, (graph*)g,1, n);
    }
    else
    {//graph6 format
        writeg6(f, (graph*)g,1, n);
        return;
    }

    //printf("  -> xxxxxxxxxx\n");

}



//0 => zahodit
int bridge_prune(void* g, int n, int maxn) 
{
    return g_prune ? g_prune(g, n, maxn) : 0;
}
int bridge_preprune(void* g, int n, int maxn) 
{
    return g_preprune ? g_preprune(g, n, maxn) : 0;
}

//settery pro C++ glue
void geng_set_outproc(outproc_cb_t f)
{
    g_outproc = f; 
}
void geng_set_prune(prune_cb_t f)
{ 
    g_prune = f; 
}
void geng_set_preprune(prune_cb_t  f)
{ 
    g_preprune = f; 
}


// uzitecne globaly z geng.c
extern int geng_mindeg, geng_maxdeg, geng_mine, geng_maxe, geng_connec;


// kdyz se pouziva -l
extern void* nauty_stats; // TODO statistiky

int  geng_get_mindeg(void)
{
    return geng_mindeg; 
}
int  geng_get_maxdeg(void)
{
     return geng_maxdeg;
}
int  geng_get_mine(void)
{ 
    return geng_mine; 
}
int  geng_get_maxe(void)
{
    return geng_maxe; 
}
int  geng_get_connec(void)
{
    return geng_connec; 
}

int  geng_flag_sparse6(void)
{
    return sparse6; 
}
int  geng_flag_graph6(void)
{
    return graph6; 
}
int  geng_flag_quiet(void)
{
    return quiet; 
}
int  geng_flag_nooutput(void)
{
    return nooutput; 
}
int  geng_flag_nautyformat(void)
{ 
    return nautyformat; 
}
int  geng_flag_canonise(void)
{ 
    return canonise; 
}


int GENG_MAIN(int argc, char* argv[]); // vznikne pres -Dmain=GENG_MAIN
int geng_run(int argc, char** argv)
{ 
    return GENG_MAIN(argc, argv); 
}