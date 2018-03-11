#include <iostream>
#include <thread>

#include "drop/network/connectors/tcp.h"

using namespace drop;

int main()
{
    connection my_connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
    std :: cout << "Connection established!" << std :: endl;
}
