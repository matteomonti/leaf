#include <iostream>
#include <fstream>

#include "drop/async/pipe.hpp"

using namespace drop;

pipe <int> my_pipe;

promise <void> f()
{
    try
    {
        while(true)
        {
            int value = co_await my_pipe.pop();
            std :: cout << value << std :: endl;
        }
    }
    catch(const pipe <int> :: exceptions :: pipe_closing &)
    {
        std :: cout << "Good night!" << std :: endl;
    }
}

int main()
{
    f();

    for(int i = 0; i < 1000; i++)
        my_pipe.push(i);
}
