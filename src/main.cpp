#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/utils/sfinae.hpp"

using namespace drop;

class x
{
public:

    int serialize()
    {
        return 44;
    }
};

constexpr auto has_serialize = sfinae :: returns <int> ([](auto && x) -> decltype(x.serialize()) {});

int main()
{
    std :: cout << has_serialize.satisfied <x> () << std :: endl;
}
