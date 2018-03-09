#include <iostream>

#include "drop/network/connection.hpp"

using namespace drop;

void server()
{
    sockets :: tcp my_acceptor;

    my_acceptor.bind(1234);
    my_acceptor.listen();

    connection :: arc my_connection = my_acceptor.accept();
    std :: cout << "Connection established!" << std :: endl;

    while(true)
        std :: cout << my_connection.receive() << std :: endl;
}

void client()
{
    sockets :: tcp my_socket;
    my_socket.connect({"127.0.0.1", 1234});

    connection :: arc my_connection = my_socket;
    std :: cout << "Connection established!" << std :: endl;

    while(true)
    {
        char buffer[1024];
        std :: cin.getline(buffer, 1024);

        my_connection.send(buffer);
    }
}

int main()
{
    server();
}
