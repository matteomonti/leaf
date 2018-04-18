#include <iostream>
#include <fstream>

#include "drop/async/pipe.hpp"

using namespace drop;

int main()
{
    pipe <int> my_pipe;

    my_pipe.push(99);
    my_pipe.push(100);

    my_pipe.pop().then([&](const int & value)
    {
        std :: cout << value << std :: endl;
        return my_pipe.pop();
    }).then([&](const int & value)
    {
        std :: cout << value << std :: endl;
        return my_pipe.pop();
    }).then([&](const int & value)
    {
        std :: cout << value << std :: endl;
    });

    my_pipe.push(101);
}
