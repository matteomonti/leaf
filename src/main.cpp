#include <iostream>

#include "drop/async/collector.hpp"

using namespace drop;

template <typename type> void print()
{
    std :: cout << __PRETTY_FUNCTION__ << std :: endl;
}

int main()
{
    print <typename collector <> :: traits :: storage <promise <void>>> ();
    print <typename collector <> :: traits :: storage <std :: array <promise <void>, 4>>> ();
    print <typename collector <> :: traits :: storage <promise <int>>> ();
    print <typename collector <> :: traits :: storage <std :: array <promise <int>, 4>>> ();
}
