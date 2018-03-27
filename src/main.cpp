#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> f(promise <void> p0)
{
    std :: cout << "Awaiting" << std :: endl;
    co_await p0;
    std :: cout << "Returning from f" << std :: endl;
}

promise <void> g(const promise <void> & p0)
{
    std :: cout << "Calling f" << std :: endl;
    try
    {
        co_await f(p0);
    }
    catch(const char * exception)
    {
        std :: cout << exception << std :: endl;
    }
    std :: cout << "Returned from f" << std :: endl;
}

int main()
{
    {
        promise <void> p0;
        g(p0);
        p0.reject("Ok, so far so good");
    }
}
