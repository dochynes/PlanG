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
void geng_clear_color_bounds(void);
void geng_set_color_bounds(int color, int lower, int upper);
const int* geng_get_current_vertex_colors(void);
const int* geng_get_output_vertex_colors(void);
int  geng_get_current_color_count(void);
int  geng_run(int argc, char** argv);
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
        ::geng_get_output_vertex_colors(),
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
    ::geng_clear_color_bounds();
    refresh_outproc_registration();
}

void setColors(int count)
{
    int color_count = std::max(0, count);

    setColorCount(color_count);
    for (int color = 0; color < color_count; ++color)
        ::geng_set_color_bounds(color, 1, MAXN);
    refresh_outproc_registration();
}

void setColorClassSizes(const std::vector<int>& sizes)
{
    setColorCount(static_cast<int>(sizes.size()));
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        int size = std::max(0, sizes[i]);
        ::geng_set_color_bounds(static_cast<int>(i), size, size);
    }
}

void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds)
{
    setColorCount(static_cast<int>(bounds.size()));
    for (std::size_t i = 0; i < bounds.size(); ++i)
    {
        int lower = std::max(0, bounds[i].first);

        int upper;
        if(bounds[i].second < 0)
        {
            upper = -1;
        }
        else
        {
            upper = std::max(lower, bounds[i].second);
        }
        ::geng_set_color_bounds(static_cast<int>(i), lower, upper);
    }
}

void setOutproc(OutprocFn f)
{
    s_outproc = std::move(f);
    refresh_outproc_registration();
}

void setPrune(PruneFn f)
{
    s_prune = std::move(f);
    if (s_prune)
        ::geng_set_prune(&c_prune);
    else
        ::geng_set_prune(nullptr);
}

void setPreprune(PrepruneFn f)
{
    s_preprune = std::move(f);
    if (s_preprune)
        ::geng_set_preprune(&c_preprune);
    else
        ::geng_set_preprune(nullptr);
}


int run(int argc, char** argv)
{
    return ::geng_run(argc, argv);
}

}
