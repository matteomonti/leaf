#include <iostream>

#include "drop/data/varint.hpp"

using namespace drop;

int main()
{
    varint :: streamer my_streamer;

    std :: cout << my_streamer.pending() << std :: endl;

    uint8_t byte = 0xc1;
    my_streamer.update(&byte, 1);

    std :: cout << my_streamer.pending() << std :: endl;

    byte = 0x63;
    my_streamer.update(&byte, 1);

    std :: cout << my_streamer.pending() << std :: endl;

    uint8_t bytes[2] = {0x49, 0x7a};
    my_streamer.update(bytes, 2);

    std :: cout << my_streamer.pending() << std :: endl;

    std :: cout << my_streamer.yield() << std :: endl;
}
