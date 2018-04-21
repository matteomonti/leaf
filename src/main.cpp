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

    std :: cout << collector <> :: traits :: is_valid <promise <void>> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <std :: array <promise <void>, 4>> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <promise <int>> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <std :: array <promise <int>, 4>> () << std :: endl;


    std :: cout << collector <> :: traits :: is_valid <void> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <std :: array <void, 4>> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <int> () << std :: endl;
    std :: cout << collector <> :: traits :: is_valid <std :: array <int, 4>> () << std :: endl;

    std :: cout << std :: endl;

    std :: cout << collector <> :: constraints :: valid() << std :: endl;
    std :: cout << collector <promise <void>> :: constraints :: valid() << std :: endl;
    std :: cout << collector <promise <int>, std :: array <promise <void>, 4>> :: constraints :: valid() << std :: endl;
    std :: cout << collector <promise <int>, std :: array <promise <void>, 4>, int> :: constraints :: valid() << std :: endl;
}
