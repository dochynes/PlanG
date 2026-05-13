#include "MyGraphLib.hpp"
#include <iostream>
#include <vector>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

using App = Generator<geng::Backend>;



static int
prune_nonplanar (const App::GraphView& g)
{
  if (boost::boyer_myrvold_planarity_test (g))
    return KEEP;
  else
    return PRUNE;
}

int main(int argc, char** argv)
{
    App app;

    app.setVertices(9);

    app.setPreprune (prune_nonplanar);

 
    return app.run();
}


