#include <iostream>

#include "drop/network/sockets/tcp.h"

using namespace drop;

int main()
{
    sockets :: tcp my_acceptor;

    my_acceptor.bind(1234);
    my_acceptor.listen();

    sockets :: tcp my_connection = my_acceptor.accept();

    std :: cout << my_connection.remote() << std :: endl;

    char buffer[1024];
    size_t received = my_connection.receive(buffer, 1024);

    buffer[received] = '\0';

    std :: cout << buffer << std :: endl;
}
