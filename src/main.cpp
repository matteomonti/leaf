#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <uint64_t> my_syncset;

    for(uint64_t i = 0; i < 10000000; i++)
    {
        if(i % 1000 == 0)
            std :: cout << "Adding " << i << std :: endl;

        my_syncset.add(i);
    }
}
