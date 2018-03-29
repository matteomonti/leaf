#include <iostream>

#include "drop/network/acceptors/tcp.h"
#include "drop/network/connectors/tcp.h"

using namespace drop;

int main()
{
    acceptors :: tcp :: async my_acceptor(1234);

    my_acceptor.on <connection> ([](const connection & connection)
    {
        connection.send(buffer("Hello World!"));
    });

    for(size_t i = 0; i < 10; i++)
    {
        connection connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
        std :: cout << connection.receive() << std :: endl;
    }
}
