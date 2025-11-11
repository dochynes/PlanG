#pragma once
#include <functional>
#include <cstdio>

namespace geng {

// registrace callbacku
void setOutproc(std::function<void(FILE* f, const void* g, int n)> f);
void setPrune(std::function<int(const void* g, int n, int maxn)> f);
void setPreprune(std::function<int(const void* g, int n, int maxn)> f); 


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