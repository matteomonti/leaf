#include <iostream>

#include "drop/async/collector.hpp"

using namespace drop;

template <typename type> void print()
{
    std :: cout << __PRETTY_FUNCTION__ << std :: endl;
}

int main()
{
    promise <void> my_void_promise;
    promise <int> my_int_promise;
    std :: array <promise <double>, 2> my_double_promises;

    collector <required <promise <void>>, until <promise <int>>, until <required <std :: array <promise <double>, 2>>>> my_collector(my_void_promise, my_int_promise, my_double_promises);
}
