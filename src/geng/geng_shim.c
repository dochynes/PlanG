#include "gtools.h"
#include <string.h>


typedef void (*outproc_cb_t)(FILE* f, void* g, int n);
typedef int (*prune_cb_t)(void* g, int n, int maxn);

static outproc_cb_t g_outproc = NULL;
static prune_cb_t g_prune = NULL;
static prune_cb_t g_preprune = NULL;

extern int sparse6, nooutput, nautyformat, canonise;
extern int geng_vertex_color_count;
extern int geng_vertex_color_lower[MAXN];
extern int geng_vertex_color_upper[MAXN];
extern int geng_current_vertex_color[MAXN];
extern int geng_canonical_vertex_color[MAXN];
extern int geng_canonical_vertex_color_active;

static const int* output_vertex_colours(void)
{
    if (canonise && geng_canonical_vertex_color_active)
        return geng_canonical_vertex_color;

    return geng_current_vertex_color;
}

static void write_vertex_colours(FILE* f, int n, const int* colours)
{
    int i;

    fputs(" colors:", f);
    for (i = 0; i < n; ++i)
    {
        fprintf(f, " %d", colours[i]);
    }
    fputc('\n', f);
}

static void write_graph_code_without_newline(FILE* f, const char* code)
{
    size_t len = strlen(code);

    if (len > 0 && code[len - 1] == '\n')
        --len;

    fwrite(code, 1, len, f);
}


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
        if (geng_vertex_color_count > 0)
        {
            write_graph_code_without_newline(f, ntos6(g, 1, n));
            write_vertex_colours(f, n, output_vertex_colours());
        }
        else
        {
            writes6(f, g, 1, n);
        }
    }
    else 
    { 
        if (geng_vertex_color_count > 0)
        {
            write_graph_code_without_newline(f, ntog6(g, 1, n));
            write_vertex_colours(f, n, output_vertex_colours());
        }
        else
        {
            writeg6(f, g, 1, n);
        }
    }
}

int bridge_outproc_active(void)
{
    return g_outproc != NULL || geng_vertex_color_count > 0;
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
void geng_set_color_count(int count)
{
    geng_vertex_color_count = count;
}
void geng_clear_color_bounds(void)
{
    int i;
    for (i = 0; i < MAXN; ++i)
    {
        geng_vertex_color_lower[i] = 0;
        geng_vertex_color_upper[i] = -1;
    }
}
void geng_set_color_bounds(int color, int lower, int upper)
{
    if (color >= 0 && color < MAXN)
    {
        geng_vertex_color_lower[color] = lower;
        geng_vertex_color_upper[color] = upper;
    }
}
const int* geng_get_current_vertex_colors(void)
{
    return geng_current_vertex_color;
}
const int* geng_get_output_vertex_colors(void)
{
    return output_vertex_colours();
}
int geng_get_current_color_count(void)
{
    return geng_vertex_color_count;
}

int GENG_MAIN(int argc, char* argv[]); // vznikne pres -DGENG_MAIN=geng_main
int geng_run(int argc, char** argv)
{ 
    return GENG_MAIN(argc, argv); 
}
