#include <stddef.h>

//typy ukazatelu
typedef int (*prefilter_cb_t)(void);
typedef int (*filter_cb_t)(int nbtot, int nbop, int doflip);

//drzaky na callbacky
static prefilter_cb_t g_prefilter_cb = NULL;
static filter_cb_t    g_filter_cb = NULL;

// trampoliny ktere volaji plantri
static int bridge_prefilter_common(void)
{
    // 1=expandovat 0=orezat vetev
    if (!g_prefilter_cb) 
        return 1;
    return g_prefilter_cb();
}

static int bridge_filter_common(int nbtot, int nbop, int doflip)
{
    //0 = nepsat ven
    if (!g_filter_cb) 
        return 0;
    return g_filter_cb(nbtot, nbop, doflip);
}


//premapovani vsech beznych pre-filter maker na jednu funkci
#define PRE_FILTER_SIMPLE       bridge_prefilter_common()
#define PRE_FILTER_MIN4         bridge_prefilter_common()
#define PRE_FILTER_BIP          bridge_prefilter_common()
#define PRE_FILTER_POLY         bridge_prefilter_common()
#define PRE_FILTER_DOUBLE       bridge_prefilter_common()
#define PRE_FILTER_ORDLOOP      bridge_prefilter_common()
#define PRE_FILTER_SPECIALLOOP  bridge_prefilter_common()
#define PRE_FILTER_QUAD         bridge_prefilter_common()
#define PRE_FILTER_MIN5         bridge_prefilter_common()


#define FILTER(nbtot, nbop, doflip) bridge_filter_common((nbtot),(nbop),(doflip))

// setter funkce
void pt_set_prefilter(prefilter_cb_t f) { g_prefilter_cb = f; }
void pt_set_filter   (filter_cb_t    f) { g_filter_cb    = f; }

// uzitecne glovbaly z plantri.c
extern int nv, ne, missing_vertex;
extern int *degree;

typedef struct EDGE EDGE;     // 
extern EDGE **firstedge;      // firstedge[v] -> hrana z vrcholu v


int  pt_nv(void) { return nv; }
int  pt_ne_oriented(void) { return ne; }          // orient 2*E
int* pt_degree_array(void) { return degree; }
int  pt_missing_vertex(void) { return missing_vertex; }
EDGE** pt_firstedge_array(void) { return firstedge; }


int plantri_run(int argc, char** argv); //TODO: v CMake bude  -Dmain=plantri_run
int pt_run(int argc, char** argv) { return plantri_run(argc, argv); } 