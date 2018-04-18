#include <iostream>
#include <fstream>

#include "drop/async/pipe.hpp"

using namespace drop;

pipe <int> my_pipe;

promise <void> f()
{
    while(true)
    {
        int value = co_await my_pipe.pop();
        if(value % 1000000 == 0)
            std :: cout << value << std :: endl;
    }
}

int main()
{
    f();

    for(int i = 0;; i++)
        my_pipe.push(i);
}
