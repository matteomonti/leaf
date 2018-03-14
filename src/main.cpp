#include <iostream>
#include <thread>

// Includes

#include "drop/network/pool.hpp"
#include "drop/network/connectors/tcp.h"

using namespace drop;

int main()
{
    pool my_pool;

    connection my_connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
    pool :: connection my_async_connection = my_pool.bind(my_connection);
}
