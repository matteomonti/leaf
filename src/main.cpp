#include <iostream>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <uint64_t> my_syncset;

    for(size_t i = 0; i < 100000; i++)
        my_syncset.add(i);

    syncset <uint64_t> :: prefix my_prefix(1255433, 0);
    std :: cout << "Target prefix:" << hash(uint64_t(1255433)) << std :: endl;

    my_syncset.enumerate(my_prefix, [](const uint64_t & element)
    {
        std :: cout << hash(element) << std :: endl;
    });
}
