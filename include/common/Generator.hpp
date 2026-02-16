#pragma once
#include "Output.hpp"
#include <string>
#include <vector>


// T = geng::Backend nebo plantri::Backend

template<typename Backend>
struct Generator : public Backend
{
    using GraphView = typename Backend::GraphView;

    static int run()
    {
        std::vector<std::string> args = Backend::prepare_args();

        std::vector<char*> c_args;

        for(auto& s :args)
        {
            c_args.push_back(s.data());
        }

        int argc = args.size();
        char** argv = c_args.data();

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
