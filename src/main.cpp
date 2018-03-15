#include <iostream>

// Includes

#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/pool.hpp"

using namespace drop;

promise <void> server(pool :: connection connection)
{
    while(true)
    {
        buffer message = co_await connection.receive();
        std :: cout << "Server received " << message << std :: endl;
        co_await connection.send(buffer("hallo"));
    }
}

promise <void> client(pool :: connection connection)
{
    while(true)
    {
        co_await connection.send(buffer("hello"));
        buffer message = co_await connection.receive();
        std :: cout << "Client received " << message << std :: endl;
    }
}

int main()
{
    pool pool;
    acceptors :: tcp :: async acceptor(1234);
    connectors :: tcp :: async connector;

    acceptor.on <connection> ([&](const connection & connection)
    {
        server(pool.bind(connection));
    });

    connector.connect({"127.0.0.1", 1234}).then([&](const connection & connection)
    {
        client(pool.bind(connection));
    });

    sleep(1_h);
}
