#pragma once
#include "Output.hpp"
#include <string>
#include <utility>
#include <vector>


// T = geng::Backend nebo plantri::Backend

template<typename Backend>
struct Generator : public Backend
{
    using GraphView = typename Backend::GraphView;

    int run()
    {
        this->apply_runtime_state();

        std::vector<std::string> args = this->prepare_args();

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
    void setPrune(Func&& f) 
    {
        this->Backend::setPrune(std::forward<Func>(f));
    }

    template <typename Func>
    void setPreprune(Func&& f) 
    {
        this->Backend::setPreprune(std::forward<Func>(f));
    }

    template <typename Func>
    void setOutproc(Func&& f) 
    {
        this->Backend::setOutproc(std::forward<Func>(f));
    }


};
