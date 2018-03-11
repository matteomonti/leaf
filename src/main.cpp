#include <iostream>
#include <thread>

#include "drop/network/connectors/tcp.h"

using namespace drop;

connectors :: tcp :: async my_connector;

promise <void> run()
{
    try
    {
        connection my_connection = co_await my_connector.connect({"127.0.0.77", 12434});
    }
    catch(...)
    {
        std :: cout << "There has been an exception" << std :: endl;
    }
}

int main()
{
    run();
    sleep(10_s);
}
