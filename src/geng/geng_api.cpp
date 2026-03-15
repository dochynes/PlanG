#include "geng/GengAPI.hpp"
#include "geng/GraphView.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <unordered_set>
#include <utility>
#include <iostream>

extern "C"
{
void geng_set_outproc(void(*)(FILE*,void*,int));
void geng_set_prune(int(*)(void*,int,int));
void geng_set_preprune(int(*)(void*,int,int));
int  geng_run(int argc, char** argv);

int  geng_get_mindeg(void);
int  geng_get_maxdeg(void);
int  geng_get_mine(void);
int  geng_get_maxe(void);
int  geng_get_connec(void);

int  geng_flag_sparse6(void);
int  geng_flag_graph6(void);
int  geng_flag_quiet(void);
int  geng_flag_nooutput(void);
int  geng_flag_nautyformat(void);
int  geng_flag_canonise(void);
}

namespace
{
using geng::GraphView;
using geng::OutprocFn;
using geng::PruneFn;
using geng::PrepruneFn;

static OutprocFn s_outproc;
static PruneFn s_prune;
static PrepruneFn s_preprune;
static std::vector<int> s_fixed_vertices;
static void c_outproc(FILE* f, void* gg, int n);
static void c_outproc_b(FILE* f,void* gg, int n);



static void refresh_outproc_registration()
{
    if(!s_fixed_vertices.empty())
    {
        ::geng_set_outproc(&c_outproc);
    }
    else if (s_outproc)
    {
        ::geng_set_outproc(&c_outproc_b);
    }
    else
    {
        ::geng_set_outproc(nullptr);
    }
}


static std::vector<graph> relabel_graph_with_tuple(const graph* src, int n, const std::vector<int>& fixed, const std::vector<int>& tuple)
{
    std::vector<int> old_to_new(n, -1);
    std::vector<char> old_used(n, 0);
    std::vector<char> new_used(n, 0);

    for (std::size_t i = 0; i < fixed.size(); ++i)
    {
        const int old_v = tuple[i];
        const int new_v = fixed[i];
        old_to_new[old_v] = new_v;
        old_used[old_v] = 1;
        new_used[new_v] = 1;
    }

    std::vector<int> old_rest;
    std::vector<int> new_rest;
    old_rest.reserve(n);
    new_rest.reserve(n);
    for (int v = 0; v < n; ++v)
    {
        if(!old_used[v]) 
        {
            old_rest.push_back(v);
        }
        if(!new_used[v])
        {
            new_rest.push_back(v);
        }
    }

    for (std::size_t i = 0; i < old_rest.size(); ++i)
    {
        old_to_new[old_rest[i]] = new_rest[i];
    }

    std::vector<graph> dst(n, 0);
    for (int u = 0; u < n; ++u)
    {
        for (int v = u + 1; v < n; ++v)
        {
            if (!ISELEMENT1(&src[u], v))
                continue;
            const int nu = old_to_new[u];
            const int nv = old_to_new[v];
            ADDELEMENT1(&dst[nu], nv);
            ADDELEMENT1(&dst[nv], nu);
        }
    }
    return dst;
}

static std::string canonical_signature_for_tuple(const graph* g, int n, const std::vector<int>& tuple)
{
    int lab[MAXN];
    int ptn[MAXN];
    int orbits[MAXN];
    statsblk stats;
    DEFAULTOPTIONS_GRAPH(options);
    setword workspace[50];

    std::vector<char> anchored(n, 0);
    int pos = 0;
    for (int v : tuple)
    {
        lab[pos] = v;
        ptn[pos] = 0;
        anchored[v] = 1;
        ++pos;
    }
    for (int v = 0; v < n; ++v)
    {
        if (anchored[v]) 
            continue;
        lab[pos] = v;
        ptn[pos] = 1;
        ++pos;
    }
    ptn[n - 1] = 0;

    options.defaultptn = FALSE;
    options.getcanon = TRUE;

    std::vector<graph> canon(n, 0);

    nauty(const_cast<graph*>(g), lab, ptn, nullptr, orbits, &options, &stats, workspace, 50, 1, n, canon.data());


    return std::string(reinterpret_cast<const char*>(canon.data()), canon.size() * sizeof(graph));

}

static std::vector<std::vector<graph>> representatives_with_fixed_vertices(const graph* g, int n, const std::vector<int>& fixed)
{
    std::vector<std::vector<graph>> reps;
    if (fixed.empty())
        return reps;
    //if (!fixed_vertices_valid_for_n(fixed, n)) return reps;

    std::unordered_set<std::string> seen;   

    const int k = fixed.size();  //delka tuple
    std::vector<int> tuple(k, -1);  // tuple[d] == ktery vrchol jsme vybrali pro pozici fixed[d]

    std::vector<char> used(n, 0);  //used[v]== 1 pokud je vrchol uz pouzity v aktualni tuple


    std::vector<int> next_candidate(k,0);

    int depth = 0; //backtracking, vybirame vrchol pro tuple[depth]

    while (depth >=0)
    {
        if(depth == k) // kandidat
        {
            const std::string sig = canonical_signature_for_tuple(g, n, tuple); // 

            bool was_inserted = seen.insert(sig).second;
            if(was_inserted)
            {
                std::vector<graph> new_graph = relabel_graph_with_tuple(g,n,fixed,tuple);
                reps.push_back(new_graph);
            }

            depth--;

            if (depth >= 0)
            {
                used[tuple[depth]] = 0;
            }

            continue;
            
        }

        bool found = false;

        for(int v = next_candidate[depth];v<n;v++)
        {
            if(used[v])
                continue;

            tuple[depth] = v;
            used[v] = 1;

            next_candidate[depth] = v + 1;

            depth++;

            if (depth < k)
                next_candidate[depth] = 0;
            
            found = true;
            break;
            
        }

         if (!found)
        {
            next_candidate[depth] = 0;
  
            depth--;

            if (depth >= 0)
            {
                int v = tuple[depth];
                used[v] = 0; 
            }
        }
    }

    return reps;

}



static void c_outproc(FILE* f, void* gg, int n)
{

    Output out(f);
    const graph* base = static_cast<const graph*>(gg);

    std::vector<std::vector<graph>> reps = representatives_with_fixed_vertices(base, n, s_fixed_vertices);
    
    for (const auto& rep : reps)
    {
        GraphView view{rep.data(), n, n};

        if(s_prune && s_prune(view) != 0) 
            continue;

        if(s_outproc)
        {
            s_outproc(out, view);
            continue;
        }

        if(geng::flagNoOutput()) 
            continue;
        if(geng::flagNautyFormat())
            continue;  
        if (geng::flagSparse6())
            ::writes6(out.raw(), const_cast<graph*>(view.data()), 1, n);
        else
            ::writeg6(out.raw(), const_cast<graph*>(view.data()), 1, n);
    }
}

static void c_outproc_b(FILE* f, void* gg, int n)
{
    if (!s_outproc)
        return;
    Output out(f);
    GraphView view{static_cast<const graph*>(gg), n, n};
    s_outproc(out, view);
}

static int c_preprune(void* gg, int n, int maxn)
{
    if (!s_fixed_vertices.empty())
        return 0;
    if (!s_preprune) 
        return 0;

    GraphView view{static_cast<const graph*>(gg), n, maxn};
    return s_preprune(view);
}

static int c_prune(void* gg, int n, int maxn)
{
    if (!s_fixed_vertices.empty()) 
        return 0;
    if (!s_prune) 
        return 0;

    GraphView view{static_cast<const graph*>(gg), n, maxn};
    return s_prune(view);
}

} // namespace

namespace geng {

void setOutproc(OutprocFn f)
{
    s_outproc = std::move(f);
    refresh_outproc_registration();
}

void setPrune(PruneFn f)
{
    s_prune = std::move(f);
    ::geng_set_prune(&c_prune);
}

void setPreprune(PrepruneFn f)
{
    s_preprune = std::move(f);
    ::geng_set_preprune(&c_preprune);
}

void setFixedVertices(const std::vector<int>& vertices)
{
    s_fixed_vertices = vertices;
    refresh_outproc_registration();
}

bool hasFixedVertices()
{
    return !s_fixed_vertices.empty();
}

int run(int argc, char** argv)
{
    return ::geng_run(argc, argv);
}

int mindeg()
{
    return ::geng_get_mindeg();
}
int maxdeg()
{
    return ::geng_get_maxdeg();
}
int mine()
{
    return ::geng_get_mine();
}
int maxe()
{
    return ::geng_get_maxe();
}
int connec()
{
    return ::geng_get_connec();
}

bool flagSparse6()
{
    return ::geng_flag_sparse6() != 0;
}
bool flagGraph6()
{
    return ::geng_flag_graph6() != 0;
}
bool flagQuiet()
{
    return ::geng_flag_quiet() != 0;
}
bool flagNoOutput()
{
    return ::geng_flag_nooutput() != 0;
}
bool flagNautyFormat()
{
    return ::geng_flag_nautyformat() != 0;
}
bool flagCanonise()
{
    return ::geng_flag_canonise() != 0;
}

}
