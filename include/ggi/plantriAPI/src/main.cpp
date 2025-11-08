#include "plantri/BridgeAPI.hpp"
#include <iostream>


int main() 
{
    //co mame : 
    /*
    
void setPrefilter(std::function<int()> f);
void setFilter(std::function<int(int,int,int)> f);


int  pt_run(int argc, char** argv);


int pt_nv();   // pocet vrcholu aktualniho grafu
int pt_ne_oriented(); pocet orientovanych hran (2xpocet hran) 
int* pt_degree_array();    ukazatel na interni pole stupnu vrcholu 
int pt_missing_vertex();    missing_vertex.  

// firstedge[v] -> EDGE*
EDGE** pt_firstedge_array();   //ukazatel na pole na pole firstedge[0 ... nv-1]


}*/
    plantri::setPrefilter([] {
        int n = plantri::pt_nv();
        int* deg = plantri::pt_degree_array();
        for (int v = 0; v < n; ++v)
            if (deg[v] < 3) return 0; // prune
        return 1;
    });

    plantri::setFilter([](int, int, int) {
        int n = plantri::pt_nv();
        EDGE** fe = plantri::pt_firstedge_array();

        //DO SOMETHING
        return 0; // nepsat ven
    });

    const char* args[] = {"plantri", "-c", "-o", "15"};
    return plantri::pt_run(4, const_cast<char**>(args));
}