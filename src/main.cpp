#include <iostream>
#include <thread>

#include "drop/network/connection.hpp"
#include "drop/chrono/time.hpp"

using namespace drop;

void server()
{
    sockets :: tcp my_acceptor;

    my_acceptor.bind(1234);
    my_acceptor.listen();

    connection my_connection = my_acceptor.accept();
    std :: cout << "Connection established!" << std :: endl;

    while(true)
        std :: cout << my_connection.receive() << std :: endl;
}

void client()
{
    sockets :: tcp my_socket;
    my_socket.connect({"127.0.0.1", 1234});

    connection my_connection = my_socket;
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
    std :: thread serverthread(server);
    sleep(1_s);
    std :: thread clientthread(client);

    sleep(10_h);
}
