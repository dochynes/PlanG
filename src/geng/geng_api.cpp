#include "geng/GengAPI.hpp"
#include "geng/GraphView.hpp"
#include <algorithm>
#include <utility>

extern "C"
{
void geng_set_outproc(void(*)(FILE*,void*,int));
void geng_set_prune(int(*)(void*,int,int));
void geng_set_preprune(int(*)(void*,int,int));
void geng_set_color_count(int count);
void geng_clear_color_target_counts(void);
void geng_set_color_target_count(int color, int count);
const int* geng_get_current_vertex_colors(void);
int  geng_get_current_color_count(void);
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
static void c_outproc(FILE* f, void* gg, int n);

static void refresh_outproc_registration()
{
    if (s_outproc)
        ::geng_set_outproc(&c_outproc);
    else
        ::geng_set_outproc(nullptr);
}

static void c_outproc(FILE* f, void* gg, int n)
{
    if (!s_outproc)
        return;

    Output out(f);
    GraphView view{
        static_cast<const graph*>(gg),
        n,
        n,
        ::geng_get_current_vertex_colors(),
        ::geng_get_current_color_count()
    };
    s_outproc(out, view);
}

static int c_preprune(void* gg, int n, int maxn)
{
    if (!s_preprune)
        return 0;

    GraphView view{
        static_cast<const graph*>(gg),
        n,
        maxn,
        ::geng_get_current_vertex_colors(),
        ::geng_get_current_color_count()
    };
    return s_preprune(view);
}

static int c_prune(void* gg, int n, int maxn)
{
    if (!s_prune)
        return 0;

    GraphView view{
        static_cast<const graph*>(gg),
        n,
        maxn,
        ::geng_get_current_vertex_colors(),
        ::geng_get_current_color_count()
    };
    return s_prune(view);
}

}

namespace geng {

void setColorCount(int count)
{
    ::geng_set_color_count(std::max(0, count));
    ::geng_clear_color_target_counts();
    refresh_outproc_registration();
}

void setColorClassSizes(const std::vector<int>& sizes)
{
    setColorCount(static_cast<int>(sizes.size()));
    for (std::size_t i = 0; i < sizes.size(); ++i)
        ::geng_set_color_target_count(static_cast<int>(i), std::max(0, sizes[i]));
}

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
