#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> my_promise;

promise <int> f()
{
    std :: cout << "Entro in f!" << std :: endl;
    co_await my_promise;

    std :: cout << "Torno in f!" << std :: endl;
    co_return 33;
}

int main()
{
    f().then([](const int & value)
    {
        std :: cout << "Done with value " << value << std :: endl;
    });

    my_promise.resolve();
}
