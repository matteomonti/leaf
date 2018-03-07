#include <iostream>

#include "drop/chrono/time.hpp"

using namespace drop;

int main()
{
    timestamp x = now;
    std :: cout << x << std :: endl;

    std :: cout << (x + 3_m) << std :: endl;
    std :: cout << (x - 44_s) << std :: endl;
}
