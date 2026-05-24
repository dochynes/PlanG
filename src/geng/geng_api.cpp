#include "geng/GengAPI.hpp"
#include "geng/GraphView.hpp"
#include <algorithm>
#include <exception>
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
static std::exception_ptr callback_exception;
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
    if (!s_outproc || callback_exception)
        return;

    try
    {
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
    catch(...)
    {
        if (!callback_exception)
            callback_exception = std::current_exception();
    }
}

static int c_preprune(void* gg, int n, int maxn)
{
    if (!s_preprune)
        return 0;
    if (callback_exception)
        return 1;

    try
    {
        GraphView view{
            static_cast<const graph*>(gg),
            n,
            maxn,
            ::geng_get_current_vertex_colors(),
            ::geng_get_current_color_count()
        };
        return s_preprune(view);
    }
    catch(...)
    {
        if (!callback_exception)
            callback_exception = std::current_exception();
        return 1;
    }
}

static int c_prune(void* gg, int n, int maxn)
{
    if (!s_prune)
        return 0;
    if (callback_exception)
        return 1;
    if (n != maxn)
        return 0;

    try
    {
        GraphView view{
            static_cast<const graph*>(gg),
            n,
            maxn,
            ::geng_get_current_vertex_colors(),
            ::geng_get_current_color_count()
        };
        return s_prune(view);
    }
    catch(...)
    {
        if(!callback_exception)
            callback_exception = std::current_exception();
        return 1;
    }
}

}

namespace geng {

void setColorCount(int count)
{
    if (count < 0)
        throw std::invalid_argument("geng::setColorCount expects count >= 0");

    ::geng_set_color_count(count);
    ::geng_clear_color_bounds();
    refresh_outproc_registration();
}

void setColors(int count)
{
    if (count < 0)
        throw std::invalid_argument("geng::setColors expects count >= 0");

    setColorCount(count);
    for (int color = 0; color < count; ++color)
        ::geng_set_color_bounds(color, 1, MAXN);
    refresh_outproc_registration();
}

void setColorClassSizes(const std::vector<int>& sizes)
{
    if(sizes.empty())
        throw std::invalid_argument("geng::setColorClassSizes expects at least one color class");

    for (int size : sizes)  
    {
        if (size <= 0)
            throw std::invalid_argument("geng::setColorClassSizes expects positive class sizes");
    }

    setColorCount(static_cast<int>(sizes.size()));
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        ::geng_set_color_bounds(static_cast<int>(i), sizes[i], sizes[i]);
    }
}

void setColorClassBounds(const std::vector<std::pair<int,int>>& bounds)
{
    if(bounds.empty())
        throw std::invalid_argument("geng::setColorClassBounds expects at least one color class");

    setColorCount(static_cast<int>(bounds.size()));
    for (std::size_t i = 0; i < bounds.size(); ++i)
    {
        int lower = bounds[i].first;
        int upper = bounds[i].second;

        if(lower < 0)
            throw std::invalid_argument("geng::setColorClassBounds expects lower bounds >= 0");
        if(upper < lower)
            throw std::invalid_argument("geng::setColorClassBounds expects upper >= lower");

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
    callback_exception = nullptr;
    int result = ::geng_run(argc, argv);
    if (callback_exception)
        std::rethrow_exception(callback_exception);
    return result;
}

}
