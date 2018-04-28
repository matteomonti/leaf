#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> my_promise;

promise <double> g()
{
    /*co_await my_promise;
    co_return 4.44;*/

    return my_promise.then([]()
    {
        return promise <double> :: resolved(4.44);
    });
}

int main()
{
    g().then([](const double & value)
    {
        std :: cout << "Resolved with value " << value << std :: endl;
    }).except([](const std :: exception_ptr & exception)
    {
        std :: cout << "Exception!" << std :: endl;
    });

    my_promise.resolve();
}
