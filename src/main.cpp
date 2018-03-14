#include <iostream>
#include <thread>

// Includes

#include "drop/network/connectors/tcp.h"

using namespace drop;

int main()
{
    connectors :: tcp :: async my_connector;
    
    my_connector.connect({"127.0.0.1", 1234}).then([](const connection &)
    {
        std :: cout << "Connected!" << std :: endl;
    });

    sleep(3_s);
}
