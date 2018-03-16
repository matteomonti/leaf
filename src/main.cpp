#include <iostream>

// Includes

#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"

using namespace drop;

crontab server_crontab;
crontab client_crontab;

promise <void> server(pool :: connection connection)
{
    for(uint64_t i = 1;; i+=2)
    {
        std :: cout << "[server] Into the receive" << std :: endl;
        uint64_t message = co_await connection.receive <uint64_t> ();
        std :: cout << "[server] Received " << message << std :: endl;
        co_await server_crontab.wait(0.1_s);
        std :: cout << "[server] Into the send" << std :: endl;
        co_await connection.send(i);
        std :: cout << "[server] Out of the send" << std :: endl;
    }
}

promise <void> client(pool :: connection connection)
{
    for(uint64_t i = 0;; i+=2)
    {
        std :: cout << "[client] Into the wait" << std :: endl;
        co_await client_crontab.wait(0.1_s);
        std :: cout << "[client] Into the send" << std :: endl;
        co_await connection.send(i);
        std :: cout << "[client] Out of the send" << std :: endl;
        uint64_t message = co_await connection.receive <uint64_t> ();
        std :: cout << "[client] Received " << message << std :: endl;
    }
}

int main()
{
    pool server_pool;
    pool client_pool;

    acceptors :: tcp :: async acceptor(1234);
    connectors :: tcp :: async connector;

    acceptor.on <connection> ([&](const connection & connection)
    {
        server(server_pool.bind(connection));
    });

    connector.connect({"127.0.0.1", 1234}).then([&](const connection & connection)
    {
        client(client_pool.bind(connection));
    });

    sleep(1_h);
}
