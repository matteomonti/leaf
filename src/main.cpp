#include <iostream>
#include <iomanip>

#include "drop/data/buffer.h"

using namespace drop;

int main()
{
    buffer my_buffer = "Hello World!";
    std :: cout << my_buffer << std :: endl;

    my_buffer[3] = 192;
    std :: cout << my_buffer << std :: endl;
}
