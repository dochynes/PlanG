#pragma once
#include "Output.hpp"


// T = geng::Backend nebo plantri::Backend

template<typename Backend>
struct Generator
{
    using GraphView = typename Backend::GraphView;

    static int run(int argc, char** argv)
    {
        return Backend::run(argc, argv);
    }

    template <typename Func>
    static void setPrune(Func&& f) 
    {
        Backend::setPrune(std::forward<Func>(f));
    }

    template <typename Func>
    static void setPreprune(Func&& f) 
    {
        Backend::setPreprune(std::forward<Func>(f));
    }

    template <typename Func>
    static void setOutproc(Func&& f) 
    {
        Backend::setOutproc(std::forward<Func>(f));
    }


};
