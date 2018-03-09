#include <iostream>
#include <thread>

#include "drop/network/connection.hpp"
#include "drop/chrono/time.hpp"

using namespace drop;

std :: mutex cmtx;

keyexchanger server_kx;
keyexchanger client_kx;

void server()
{
    sockets :: tcp my_acceptor;

    my_acceptor.bind(1234);
    my_acceptor.listen();

    connection my_connection = my_acceptor.accept();

    cmtx.lock();
    std :: cout << "Server: connection established!" << std :: endl;
    cmtx.unlock();

    my_connection.authenticate(server_kx, client_kx.publickey());

    cmtx.lock();
    std :: cout << "Server: connection authenticated!" << std :: endl;
    cmtx.unlock();

    while(true)
        std :: cout << my_connection.receive <uint64_t> () << std :: endl;
}

void client()
{
    sockets :: tcp my_socket;
    my_socket.connect({"127.0.0.1", 1234});

    connection my_connection = my_socket;

    cmtx.lock();
    std :: cout << "Client: connection established!" << std :: endl;
    cmtx.unlock();

    my_connection.authenticate(client_kx, server_kx.publickey());

    cmtx.lock();
    std :: cout << "Client: connection authenticated!" << std :: endl;
    cmtx.unlock();

    while(true)
    {
        uint64_t n;
        std :: cin >> n;
        my_connection.send(n);
    }
}

int main()
{
    std :: thread serverthread(server);
    sleep(1_s);
    std :: thread clientthread(client);

    sleep(10_h);
}
