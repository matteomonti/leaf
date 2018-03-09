#include <iostream>

#include "drop/data/buffer.hpp"

using namespace drop;

int main()
{
    buffer :: streamer my_streamer(41);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "hello", 5);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "world", 5);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "how", 3);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "is", 2);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "life?", 5);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "amazing", 7);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "day", 3);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "today", 5);

    std :: cout << my_streamer.pending() << std :: endl;
    my_streamer.update((uint8_t *) "right?", 6);

    std :: cout << my_streamer.pending() << std :: endl;

    buffer result = my_streamer.yield();
    std :: cout << result << std :: endl;
}
