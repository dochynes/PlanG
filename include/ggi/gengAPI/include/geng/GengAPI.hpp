#pragma once
#include <functional>
#include <cstdio>

namespace geng {

struct GraphView;

using OutprocFn = std::function<void(FILE* f, const GraphView& g)>;
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


//TODO pridat helpery do budoucna jako edgeCount,hasEdge(u,v), degree(vrchol u) ....


} 