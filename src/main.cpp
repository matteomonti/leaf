#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <int> f()
{
    co_return 33;
}

int main()
{
    f().then([](const int & value)
    {
        std :: cout << "Done with value " << value << std :: endl;
    });
}
