#include <iostream>
#include <thread>

#include "drop/network/acceptors/tcp.hpp"

using namespace drop;

int main()
{
    acceptors :: tcp :: async my_acceptor(1234);
    my_acceptor.on <connection> ([](const connection & connection)
    {
        std :: cout << "Connection incoming!" << std :: endl;
        connection.send(buffer("Hello! How's life?"));
    });

    sleep(15_s);

    my_acceptor.reset();

    sleep(1_h);
}
