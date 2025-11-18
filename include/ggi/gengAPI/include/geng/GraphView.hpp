#pragma once
extern "C" {
#include "nauty.h"   //  typ graph
}

namespace geng {

struct GraphView 
{
    const graph* g_;   
    int n_;            
    int maxn_;         
        
    int num_vertices() const
    { 
        return n_;
    }
    int maxn() const 
    { 
        return maxn_; 
    }
    int m() const 
    { 
        return SETWORDSNEEDED(maxn_); 
    }

    const graph* data() const
    {
         return g_; 
    }
};

} // namespace geng