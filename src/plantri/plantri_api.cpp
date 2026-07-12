#include "plantri/PlantriAPI.hpp"
#include "common/Output.hpp"
#include <exception>
#include <utility>
#include <functional>

// C++ strana bridge pro backend plantri. Soubor drzi C++ callbacky,
// registruje C trampoliny do plantri_shim.c a prevadi interni stav plantri
// na GraphView pouzivany verejnym API.

extern "C" {

// Funkce poskytovane souborem plantri_shim.c.

void  pt_set_prefilter(int (*f)(void));
void  pt_set_filter(int (*f)(int,int,int));
int   pt_run(int argc, char** argv);
void  pt_write_current_graph(FILE* f, int doflip);

int   pt_nv(void);
int   pt_ne_oriented(void);
int*  pt_degree_array(void);
int   pt_missing_vertex(void);

EDGE** pt_firstedge_array(void);

int pt_maxnv(void);

FILE* pt_outfile(void);

void disable_summary(void);
void enable_summary(void);


}


namespace {

    // callbacky od uživatele. Musí být globální/statické,
    // protože původní plantri callback mechanismus neumí nést stav instance objektu
    plantri::PrefilterFn g_cpp_prefilter;
    plantri::FilterFn g_cpp_filter;
    plantri::OutprocFn g_cpp_outproc;

    // místo, kam si dočasně uložíme výjimku vyhozenou uvnitř uživatelského callbacku
    // Když C++ callback hodí výjimku, nesmí výjimka volně proletět přes C funkce z plantri.c
    std::exception_ptr callback_exception;

    constexpr int prune = 1;
    constexpr int keep = 0;

    // hodnota doflip pro aktualni graf v outproc callbacku
    int g_current_doflip = 0;


    // Zabali aktualni interni graf plantri do GraphView
    // ulozi ukazatele na data a dulezite hodnoty
    inline plantri::GraphView make_view()
    {
        plantri::GraphView v{
            ::pt_firstedge_array(),
            ::pt_degree_array(),
            ::pt_nv(),
            ::pt_ne_oriented(),
            ::pt_missing_vertex(),
            ::pt_maxnv()
        };
        return v;
    }

    // Trampolina volana z PRE_FILTER_* maker v plantri_shim.c.
    int c_prefilter_trampoline() 
    {
        
        if (callback_exception)
            return prune;

        try
        {
            if(!g_cpp_prefilter)
            {
                return prune;
            }
            auto view = make_view();
            return g_cpp_prefilter(view);
        }
        catch(...) 
        { 
            if (!callback_exception)
                callback_exception = std::current_exception();
            return prune;
        }
    }


    // Trampolina volana z FILTER makra. Nejprve spusti uzivatelsky filter,
    // potom pripadne outproc. Pokud outproc existuje, vraci PRUNE, aby plantri
    // graf uz samo nevypisovalo podruhe.
    int c_filter_trampoline(int nbtot, int nbop, int doflip) 
    {

        (void)nbtot;
        (void)nbop;
        if (callback_exception)
            return prune;        //1 == PRUNE

        try
        {
            auto view = make_view();
            int should_prune = 0; //  1

            if (g_cpp_filter)
            {
                
                should_prune = g_cpp_filter(view);
            }
            

            if (should_prune == 1) 
            {
                return prune; 
            }

            if (g_cpp_outproc)
            {
                
                FILE* f = ::pt_outfile();
                Output out(f);
                g_current_doflip = doflip;
                g_cpp_outproc(out, view);
                g_current_doflip = 0;
                
               
                return prune;
            }

            return keep;

        }
        catch(...) 
        {
            g_current_doflip = 0;
            if (!callback_exception)
                callback_exception = std::current_exception();
            return prune;
        }
    }

    // Pomocna funkce pro operator<<, zapise aktualni graf pres plantri.
    void write_current_graph(FILE* f)
    {
        ::pt_write_current_graph(f, g_current_doflip);
    }

    void refresh_filter_registration()
    {
        if (g_cpp_filter || g_cpp_outproc)
            ::pt_set_filter(&c_filter_trampoline);
        else
            ::pt_set_filter(nullptr);
    }
}


namespace plantri {

// Registrace uzivatelskych callbacku do C shimu.
void setPrefilter(PrefilterFn f) 
{
    g_cpp_prefilter = std::move(f);
    if (g_cpp_prefilter)
        ::pt_set_prefilter(&c_prefilter_trampoline);
    else
        ::pt_set_prefilter(nullptr);
}

void setFilter(FilterFn f) 
{
    g_cpp_filter = std::move(f);
    refresh_filter_registration();
}

void setOutproc(OutprocFn f)
{
    g_cpp_outproc = std::move(f);
    refresh_filter_registration();

    if (g_cpp_outproc)
        ::disable_summary();
    else
        ::enable_summary();
}

// Umoznuje v outproc callbacku pouzit "out << g" 
Output& operator<<(Output& out, const GraphView& g)
{
    (void)g;

    if (!out.raw())
    {
        return out;
    }

    write_current_graph(out.raw());

    return out;
}

// Spusti puvodni plantri
int pt_run(int argc, char** argv) 
{
    callback_exception = nullptr;
    int result = ::pt_run(argc, argv);
    if (callback_exception)
        std::rethrow_exception(callback_exception);
    return result;
}

// funkce pouzivane pri tvorbe GraphView
int pt_nv()
{ 
    return ::pt_nv(); 
}
int pt_ne_oriented()
{ 
    return ::pt_ne_oriented(); 
}
const int* pt_degree_array()
{
    return ::pt_degree_array(); 
}
int pt_missing_vertex()
{ 
    return ::pt_missing_vertex(); 
}

const EDGE* const* pt_firstedge_array() 
{ 
    return ::pt_firstedge_array(); 
}

int pt_maxnv() 
{ 
    return ::pt_maxnv(); 
}

}
