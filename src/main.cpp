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

    std :: cout << std :: endl;

    print <typename collector <> :: traits :: exception <promise <void>>> ();
    print <typename collector <> :: traits :: exception <std :: array <promise <void>, 4>>> ();
    print <typename collector <> :: traits :: exception <promise <int>>> ();
    print <typename collector <> :: traits :: exception <std :: array <promise <int>, 4>>> ();

    std :: cout << std :: endl;

    std :: cout << collector <> :: traits :: valid <promise <void>> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <std :: array <promise <void>, 4>> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <promise <int>> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <std :: array <promise <int>, 4>> () << std :: endl;


    std :: cout << collector <> :: traits :: valid <void> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <std :: array <void, 4>> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <int> () << std :: endl;
    std :: cout << collector <> :: traits :: valid <std :: array <int, 4>> () << std :: endl;
}
