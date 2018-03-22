#include <iostream>
#include <unordered_map>

#include "poseidon/network/directory.hpp"

using namespace drop;
using namespace poseidon;

int main()
{
    directory :: server server(7777);

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    directory :: client alice({"127.0.0.1", 7777}, connector, pool, crontab);
    alice.on <directory :: connection> ([](const directory :: connection & connection)
    {
        std :: cout << "[alice] Connection incoming from " << connection.identifier() << std :: endl;
    });

    directory :: client bob({"127.0.0.1", 7777}, connector, pool, crontab);
    bob.on <directory :: connection> ([](const directory :: connection & connection)
    {
        std :: cout << "[bob] Connection incoming from " << connection.identifier() << std :: endl;
    });

    std :: cout << "Event listeners added" << std :: endl;

    sleep(1_s);

    bob.connect(alice.identifier()).then([](const directory :: connection & connection)
    {
        std :: cout << "[bob] Connection established with " << connection.identifier() << std :: endl;
    });

    sleep(10_h);
}
