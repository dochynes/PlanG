#include "MyGraphLib.hpp"

#include <iostream>

// Tests that multiple Generator<geng::Backend> instances can be configured independently and run 
// sequentially in one process. The middle run has no output callback, so it also
// verifies that callbacks from previous runs are not reused accidentally.

using App = Generator<geng::Backend>;

int main()
{
    App a;
    App clear_callbacks;
    App b;

    int count_a = 0;
    int count_b = 0;

    a.setVertices(4);
    a.setConnected();
    a.setOutproc([&](Output&, const App::GraphView&){
        count_a++;
    });

    clear_callbacks.setVertices(3);
    clear_callbacks.setNoOutput();

    b.setVertices(4);
    b.setTriangleFree();
    b.setOutproc([&](Output&, const App::GraphView&){
        count_b++;
    });

    int result_a = a.run();
    int after_a = count_a;
    int result_clear = clear_callbacks.run();
    int result_b = b.run();

    std::cout << "connected_n4=" << count_a << "\n";
    std::cout << "triangle_free_n4=" << count_b << "\n";

    if (result_a != 0 || result_clear != 0 || result_b != 0)
        return 1;

    if (count_a != after_a)
        return 1;

    if (count_a <= 0 || count_b <= 0)
        return 1;

    if (count_a == count_b)
        return 1;
    else
        return 0;
    
}
