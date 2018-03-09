#include <iostream>

#include "drop/network/sockets/tcp.hpp"

#include "drop/data/buffer.hpp"
#include "drop/data/varint.hpp"

using namespace drop;

int main()
{
    buffer :: streamer my_streamer(99);

    sockets :: tcp my_acceptor;
    my_acceptor.bind(1234);
    my_acceptor.listen();

    sockets :: tcp my_connection = my_acceptor.accept();
    std :: cout << "Connection established" << std :: endl;

    while(my_streamer.pending())
    {
        my_connection.receive(my_streamer);
        std :: cout << "Received chunk, still " << my_streamer.pending() << " pending." << std :: endl;
    }

    buffer my_buffer = my_streamer.yield();
    std :: cout << my_buffer << std :: endl;

    my_connection.close();
    my_acceptor.close();
}
