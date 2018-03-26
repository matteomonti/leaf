#include <iostream>

#include "drop/network/pool.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"

using namespace drop;

void server(acceptors :: tcp :: async & my_acceptor, pool & my_pool, keyexchanger & client, keyexchanger & server)
{
    my_acceptor.on <connection> ([&](connection my_sync_connection) -> promise <void>
    {
        pool :: connection my_connection = my_pool.bind(my_sync_connection);
        co_await my_connection.authenticate(server, client.publickey());
    });
}

promise <void> client(connectors :: tcp :: async & my_connector, pool & my_pool, keyexchanger & client, keyexchanger & server)
{
    try
    {
        connection my_sync_connection = co_await my_connector.connect({"127.0.0.1", 1234});
        pool :: connection my_connection = my_pool.bind(my_sync_connection);

        std :: cout << "Authenticating" << std :: endl;
        co_await my_connection.authenticate(client, server.publickey());
        std :: cout << "Authenticated" << std :: endl;
    }
    catch(const std :: exception & exception)
    {
        std :: cout << "There was an exception: " << exception.what() << std :: endl;
    }
}

int main()
{
    pool my_pool;
    connectors :: tcp :: async my_connector;
    acceptors :: tcp :: async my_acceptor(1234);

    keyexchanger kxclient;
    keyexchanger kxserver;

    server(my_acceptor, my_pool, kxclient, kxserver);
    client(my_connector, my_pool, kxclient, kxserver);

    sleep(10_h);
}
