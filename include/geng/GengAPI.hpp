#pragma once
#include <functional>
#include <cstdio>
#include "gtools.h"
#include "common/Output.hpp"
#include "geng/GraphView.hpp"

namespace geng {


struct GraphView;

inline Output& operator<<(Output& out, const GraphView& g)
{
    if(out.raw())
        ::writeg6(out.raw(),const_cast<graph*>(g.data()),1,g.num_vertices());
    
    return out;
}

using OutprocFn = std::function<void(Output& out, const GraphView& g)>;
using PruneFn = std::function<int (const GraphView& g)>;
using PrepruneFn = std::function<int (const GraphView& g)>;

// registrace callbacku
void setOutproc(OutprocFn);
void setPrune(PruneFn);
void setPreprune(PrepruneFn); 


int run(int argc, char** argv);

// globaly/flagy
int mindeg();
int maxdeg();
int mine();
int maxe();
int connec();

bool flagSparse6();
bool flagGraph6();
bool flagQuiet();
bool flagNoOutput();
bool flagNautyFormat();
bool flagCanonise();


struct Backend
{

    using ViewType = geng::GraphView;
    
    static void setPrune(std::function<int(const ViewType&)> f )
    {
        geng::setPrune(f);
    }

    static void setPreprune(std::function<int(const ViewType&)> f)
    {
        geng::setPreprune(f);
    }

    static void setOutproc(std::function<void(Output&,const ViewType&)> f)
    {
        geng::setOutproc(f);
    }

    static int run(int argc, char** argv)
    { 
        return geng::run(argc, argv); 
    }
};


//TODO pridat helpery do budoucna jako edgeCount,hasEdge(u,v), degree(vrchol u) ....


} 