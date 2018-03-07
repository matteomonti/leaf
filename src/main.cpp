#include <iostream>

#include "drop/chrono/time.hpp"

using namespace drop;

int main()
{
    std :: cout << "Hello..." << std :: endl;
    sleep(3_s);
    std :: cout << "... world! :)" << std :: endl;

    std :: cout << "I'll wait until..." << std :: endl;
    sleep(timestamp(now) + 4_s);
    std :: cout << "... the time is over! :D" << std :: endl;
}
