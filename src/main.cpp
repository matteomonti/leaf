#include <iostream>

#include "drop/async/collector.hpp"

using namespace drop;

template <typename type> void print()
{
    std :: cout << __PRETTY_FUNCTION__ << std :: endl;
}

int main()
{
    collector <required <promise <void>>, until <promise <int>>, until <required <std :: array <promise <double>, 44>>>> my_collector;

    // my_collector.get <0> ();
    // my_collector.get <1> ();
    my_collector.get <2> (33);
}
