#include <stddef.h>
#include <stdio.h>

// Tento soubor se vklada do puvodniho plantri.c pres makro PLUGIN.
// Slouzi jako C bridge mezi plantri a C++ wrapperem: registruje callbacky,
// zpristupnuje aktualni graf a poskytuje vstupni body pro spusteni generatoru
// a zapis grafu z uzivatelskeho outproc callbacku.

// Typy C callbacku, ktere sem registruje C++ vrstva z plantri_api.cpp.
typedef int (*prefilter_cb_t)(void);
typedef int (*filter_cb_t)(int nbtot, int nbop, int doflip);

// Aktualne zaregistrovane callbacky. NULL znamena, ze callback neni nastaven.
static prefilter_cb_t g_prefilter_cb = NULL;
static filter_cb_t g_filter_cb = NULL;

// Trampoliny prevadeji navratove hodnoty mezi PlanG a plantri.
// PlanG pouziva KEEP = 0, PRUNE = 1; plantri ocekava 1 = pokracovat,
// 0 = zahodit vetev nebo graf.
static int bridge_prefilter_common(void)
{
    if (!g_prefilter_cb) 
        return 1;
    return !g_prefilter_cb();
}

static int bridge_filter_common(int nbtot, int nbop, int doflip)
{
    if (!g_filter_cb) 
        return 1;
    return !g_filter_cb(nbtot, nbop, doflip);
}


// plantri ma ruzna PRE_FILTER_* makra pro ruzne rezimy generovani.
// Vsechny bezne rezimy mapujeme na jednu prefilter trampolinu.
#define PRE_FILTER_SIMPLE bridge_prefilter_common()
#define PRE_FILTER_MIN4 bridge_prefilter_common()
#define PRE_FILTER_BIP bridge_prefilter_common()
#define PRE_FILTER_POLY bridge_prefilter_common()
#define PRE_FILTER_DOUBLE bridge_prefilter_common()
#define PRE_FILTER_ORDLOOP bridge_prefilter_common()
#define PRE_FILTER_SPECIALLOOP bridge_prefilter_common()
#define PRE_FILTER_QUAD bridge_prefilter_common()
#define PRE_FILTER_MIN5 bridge_prefilter_common()


// FILTER se vola pri nalezeni hotoveho grafu.
#define FILTER(nbtot, nbop, doflip) bridge_filter_common((nbtot),(nbop),(doflip))

// Registracni funkce volane z plantri_api.cpp.
void pt_set_prefilter (prefilter_cb_t f) { g_prefilter_cb = f; }
void pt_set_filter (filter_cb_t f) { g_filter_cb = f; }

// Vybrane globalni promenne z plantri.c. C++ GraphView z nich cte aktualni
// graf.
extern int nv, ne, missing_vertex;
extern int degree[];

typedef struct e EDGE;
extern EDGE *firstedge[];     // firstedge[v] -> hrana z vrcholu v

extern int maxnv;
extern FILE* outfile;
extern int dosummary;

// Pri vlastnim outproc callbacku nechceme standardni summary vypis plantri.
void disable_summary(void)
{
    dosummary = -1;
}

void enable_summary(void)
{
    dosummary = 0;
}


// Accessor funkce pro C++ wrapper.
int pt_nv(void) { return nv; }
int pt_ne_oriented(void) { return ne; }          // orient 2*E
int* pt_degree_array(void) { return degree; }
int pt_missing_vertex(void) { return missing_vertex; }
EDGE** pt_firstedge_array(void) { return firstedge; }

int pt_maxnv(void) { return maxnv; }


// Makefile prejmenuje main z plantri.c na plantri_run.
int plantri_run(int argc, char** argv);
int pt_run(int argc, char** argv) { return plantri_run(argc, argv); } 

FILE* pt_outfile(void) { return outfile; }

// Obaly kolem vystupnich funkci plantri pouzite pro operator<< a outproc.
static void write_planar_code(FILE *f, int doflip);
void pt_write_planar_code(FILE* f, int header)
{
    write_planar_code(f, header);
}

void pt_write_current_graph(FILE* f, int doflip)
{
    if (dswitch)
        (*write_dual_graph)(f, doflip);
    else
        (*write_graph)(f, doflip);
}
