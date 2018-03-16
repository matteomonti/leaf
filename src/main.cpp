#include <iostream>

#include "drop/network/pool.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/chrono/crontab.h"

using namespace drop;

crontab global_crontab;

promise <void> server(const pool :: connection & connection)
{
    while(true)
    {
        uint64_t message = co_await connection.receive <uint64_t> ();
        std :: cout << "[server] Received " << message << std :: endl;

        co_await connection.send(message + 1);
        std :: cout << "[server] Sent" << std :: endl;
    }
}

promise <void> client(const pool :: connection & connection)
{
    co_await connection.send <uint64_t> (0);
    std :: cout << "[client] Sent" << std :: endl;

    while(true)
    {
        std :: cout << "[client] Receiving" << std :: endl;
        promise <uint64_t> receive_promise = connection.receive <uint64_t> ();
        std :: cout << "[client] Promise obtained" << std :: endl;

        std :: cout << "[client] Waiting on receive" << std :: endl;
        uint64_t message = co_await receive_promise;

        std :: cout << "[client] Received " << message << std :: endl;

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
