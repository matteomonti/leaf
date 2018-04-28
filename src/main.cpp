#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> my_promise;

promise <double> g()
{
    co_await my_promise;
    co_return 4.44;
}

promise <int> f()
{
    int i = co_await g();
    throw "Tua madre puzza!";

    co_return i;
}

int main()
{
    f();
    my_promise.resolve();
}
