#include <iostream>

#include "drop/network/pool.hpp"
#include "vine/dialers/local.h"

using namespace drop;
using namespace vine;

int main()
{
    drop :: pool pool;
    dialers :: local :: server server;

    dialers :: local :: client alice(server);
    dialers :: local :: client bob(server);

    alice.on <dial> ([&](const dial & dial)
    {
        std :: cout << "Connection incoming from " << dial.identifier() << std :: endl;
        pool.bind(dial).send(buffer("Hello World")).then([]()
        {
            std :: cout << "Message sent" << std :: endl;
        });
    });

    buffer message = bob.connect(alice.identifier()).receive();
    std :: cout << "Received: " << message << std :: endl;
}
