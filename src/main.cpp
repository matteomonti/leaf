#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> f()
{
    co_return;
}

int main()
{
    f().then([]()
    {
        std :: cout << "Done!" << std :: endl;
    });
}
