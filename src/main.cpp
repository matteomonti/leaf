#include <iostream>

#include "drop/chrono/time.h"

using namespace drop;

int main()
{
    timestamp start = now;

    uint64_t n = 1;

    for(uint64_t i = 0; i < 1000000000ull; i++)
        n *= 3;

    timestamp stop = now;

    std :: cout << n << std :: endl;
    std :: cout << (stop - start) << std :: endl;
}
