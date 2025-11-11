#include "plantri/BridgeAPI.hpp"
#include <iostream>


static constexpr int MAXDEG = 6;

/* Test that vertices a,b of degree 3 are at the opposite
   points of two adjacent faces */
static bool commonedge(int a, int b) {
    EDGE** fe = plantri::pt_firstedge_array();
    const EDGE* e = fe[a];
    if (!e) return false;
    for (int t=0; t<3; ++t) {
        const EDGE* x = e->invers->next->next;
        if (x && x->end == b) return true;
        e = e->next;
        if (e == fe[a]) break;
    }
    return false;
}

// jak by slo napsat plugin(existujici) plantri_maxdeg, (logika vygenerovana pomoci AI). 
int main() 
{

    // Prefilter: heuristika pro casne prorezani
    plantri::setPrefilter([] {
        
        const int n    = plantri::pt_nv();
        const int maxn = plantri::pt_maxnv();
        const int levs = maxn - n;  

        int* deg = plantri::pt_degree_array();

        int excess = 0;  // kolik stupňů navíc musíme stáhnout, aby deg<=MAXDEG
        int d3 = 0, d4 = 0;
        int d3a = -1, d3b = -1;

        for (int v=0; v<n; ++v) {
            if (deg[v] == 3) {
                ++d3; d3a = d3b; d3b = v;
            } else if (deg[v] == 4) {
                ++d4;
            } else if (deg[v] > MAXDEG) {
                excess += (deg[v] - MAXDEG);
            }
        }

        // Pluginová pravidla:
        if (excess == 0) return 1;             // nic není třeba stahovat
        if (d3 > 2) return 0;                   // (2) víc než 2 „trojky“ -> nikdy nesnížíme
        if (d3 == 2 && !commonedge(d3a, d3b))   // (3) dvě „trojky“ nesdílí hranu
            return 0;
        if (d3 > 0 && excess >= levs)           // (1) + (4) už to nestihneme stáhnout
            return 0;

        int i = d3 + d3 + d4;                   // i = 2*#deg3 + #deg4
        if (i > 0 && excess > levs - i + 2)     // (4) omezení na E3/E4/E5
            return 0;

        return 1; // větev má šanci -> expanduj dál
        
       return 1;
    });

    plantri::setFilter([](int, int, int) {

        int n = plantri::pt_nv();
        int* deg = plantri::pt_degree_array();
        for (int v = 0; v < n; ++v)
            if (deg[v] > MAXDEG) return 0;   // zahodit
        return 1;                            // ZAPSAT na stdout 
    });

    const char* args[] = {"plantri","-g","22"};
    return plantri::pt_run(3, const_cast<char**>(args));




    //TODO graphview : //bez kopirovani
    // pocet vrcholu
    // stupen vrcholu
    // iterace sousedu vrcholu

    // + navic rozsirime pro plantri EmbadingView(automorphismInfo)

    //udelat bridge pro geng/nauty analogicky? (dosahnout graphView nad graph*)

    //dekorace az na vystupnich grafech (koreneni, obarveni...)?
}

//testovani  // plantri","-g","22" s maxdeg 6
//API 3443 triangulations written to stdout; cpu=3.64 sec
//plantri_maxdeg  3443 triangulations written to stdout; cpu=3.65 sec