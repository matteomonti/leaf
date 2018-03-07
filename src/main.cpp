#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> semaphore;

promise <void> f()
{
    for(uint64_t i = 0;; i++)
    {
        if(i % 1000000 == 0)
            std :: cout << i << std :: endl;

        semaphore = promise <void> ();
        co_await semaphore;
    }

    co_return;
}

int main()
{
    f();

    while(true)
        semaphore.resolve();
}
