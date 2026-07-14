#pragma once
#include "Output.hpp"
#include <string>
#include <utility>
#include <vector>


// Spolecny wrapper nad backendy geng::Backend a plantri::Backend.
// Backend uklada konfiguraci generatoru, Generator ji pri run() prevede na
// argv, prenese runtime stav (callbacky apod.) a spusti prislusny C generator.
// Backend musi poskytovat prepare_args(), apply_runtime_state() a run(argc,argv).
template<typename Backend>
struct Generator : public Backend
{
    using GraphView = typename Backend::GraphView;

    int run()
    {
        std::vector<std::string> args = this->prepare_args();
        this->apply_runtime_state();

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
    void setFilter(Func&& f)
    {
        this->Backend::setFilter(std::forward<Func>(f));
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
