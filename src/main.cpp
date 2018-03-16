#include <iostream>

#include "drop/network/pool.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/chrono/crontab.h"

using namespace drop;

crontab global_crontab;

promise <void> server(pool :: connection connection)
{
    while(true)
    {
        uint64_t message = co_await connection.receive <uint64_t> ();
        std :: cout << "[server] Received " << message << std :: endl;

        co_await global_crontab.wait(0.1_s);
        co_await connection.send(message + 1);
    }
}

promise <void> client(pool :: connection connection)
{
    co_await connection.send <uint64_t> (0);

    while(true)
    {
        uint64_t message = co_await connection.receive <uint64_t> ();
        std :: cout << "[client] Received " << message << std :: endl;

        co_await global_crontab.wait(0.1_s);
        co_await connection.send(message + 1);
    }
}

int main()
{
    pool pool;
    acceptors :: tcp :: sync acceptor(1234);

    connection client_connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
    connection server_connection = acceptor.accept();

    server(pool.bind(server_connection));
    client(pool.bind(client_connection));

    sleep(10_h);
}
