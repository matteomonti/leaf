#include <iostream>
#include <thread>

// Includes

#include "drop/network/pool.hpp"
#include "drop/network/connectors/tcp.h"

using namespace drop;

int main()
{
    connection my_connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});

    std :: cout << "Connected" << std :: endl;

    my_connection.send(buffer("Hello World!"));
    std :: cout << my_connection.receive() << std :: endl;
}
