#pragma once
//#include "plantri/BridgeAPI.hpp"
namespace plantri
{

    

    struct GraphView{
    EDGE** firstedge;
    int*   degree;
    int    nv;
    int    ne_oriented;
    int    missing_vertex;
    int    maxnv;

    int num_vertices() const 
    {
        return nv;
    }
    int num_edges() const 
    {
        return ne_oriented / 2; 
    }
};
}