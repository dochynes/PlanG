#include "gtools.h"
#include <string.h>

// C shim mezi puvodnim geng z nauty a C++ API PlanG.
// Makefile tento soubor pripojuje pri kompilaci geng.c pomoci -include a
// predefinuje makra OUTPROC, OUTPROC_ACTIVE, PRUNE, PREPRUNE a GENG_MAIN.
// Diky tomu muze C++ API pouzit callbacky bez uprav geng.c.

typedef void (*outproc_cb_t)(FILE* f, void* g, int n);
typedef int (*graph_predicate_cb_t)(void* g, int n, int maxn);

// Callbacky, ktere nastavuje C++ API pred spustenim gengu
static outproc_cb_t g_outproc = NULL;
static graph_predicate_cb_t g_filter = NULL;
static graph_predicate_cb_t g_preprune = NULL;

// Globalni promenne z geng.c a z doplneni rozhrani pro barvy
extern int sparse6, nooutput, nautyformat, canonise;
extern int geng_vertex_color_count;
extern int geng_vertex_color_lower[MAXN];
extern int geng_vertex_color_upper[MAXN];
extern int geng_current_vertex_color[MAXN];
extern int geng_canonical_vertex_color[MAXN];
extern int geng_canonical_vertex_color_active;

// Pri kanonickem vystupu pouzijeme barvy v kanonickem poradi vrcholu
static const int* output_vertex_colours(void)
{
    if (canonise && geng_canonical_vertex_color_active)
        return geng_canonical_vertex_color;

    return geng_current_vertex_color;
}

// dopíše za běžný výstup grafu seznam barev vrcholů
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

// Zapisuje kod grafu bez konce radku, aby za nej slo pripojit barvy
static void write_graph_code_without_newline(FILE* f, const char* code)
{
    size_t len = strlen(code);

    if (len > 0 && code[len - 1] == '\n')
        --len;

    fwrite(code, 1, len, f);
}


// OUTPROC callback volany gengem pro kazdy vystupni graf. Pokud je zaregistrovan
// C++ outproc callback, preda rizeni jemu. Jinak obsluhuje textove vystupy
// graph6/sparse6 a umi k nim dopsat barvy; binarni nauty format se o barvy
// nerozsiruje
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


// 1 = zahodit graf
int bridge_filter(graph* g, int n, int maxn)
{
    return g_filter ? g_filter((void*)g, n, maxn) : 0;
}

int bridge_preprune(graph* g, int n, int maxn) 
{
    return g_preprune ? g_preprune((void*)g, n, maxn) : 0;
}


// Funkce volane z C++ API pro nastaveni callbacku a barev
void geng_set_outproc(outproc_cb_t f)
{ 
    g_outproc = f; 
}
void geng_set_filter(graph_predicate_cb_t f)
{
     g_filter = f;
}
void geng_set_preprune(graph_predicate_cb_t f)
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

// GENG_MAIN je puvodni main z geng.c prejmenovany pri kompilaci pomoci
// -DGENG_MAIN=geng_main. C++ vrstva tak muze spustit generator jako funkci
int GENG_MAIN(int argc, char* argv[]);
int geng_run(int argc, char** argv)
{ 
    return GENG_MAIN(argc, argv); 
}
