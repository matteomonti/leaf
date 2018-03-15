#include <iostream>

// Includes

#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"

using namespace drop;

promise <void> run(connectors :: tcp :: async & my_connector, pool & my_pool)
{
    connection my_connection = co_await my_connector.connect({"127.0.0.1", 1234});
    pool :: connection my_pool_connection = my_pool.bind(my_connection);

    co_await my_pool_connection.send(buffer("Hello World!"));
    std :: cout << "Send successful" << std :: endl;

    buffer response = co_await my_pool_connection.receive();
    std :: cout << "Response was: " << response << std :: endl;

    std :: cout << "Quitting" << std :: endl;
}

int main()
{
    pool my_pool;
    connectors :: tcp :: async my_connector;

    run(my_connector, my_pool).then([]()
    {
        std :: cout << "Quitted" << std :: endl;
    });

    sleep(1_h);
}
