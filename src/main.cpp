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
    alice.on <connection> ([](const connection & connection)
    {
        std :: cout << "[alice] Connection incoming!" << std :: endl;
    });

    directory :: client bob({"127.0.0.1", 7777}, connector, pool, crontab);
    bob.on <connection> ([](const connection & connection)
    {
        std :: cout << "[bob] Connection incoming!" << std :: endl;
    });

    std :: cout << "Event listeners added" << std :: endl;

    sleep(1_s);

    bob.lookup(alice.identifier()).then([](const auto & entry)
    {
        std :: cout << entry.time << std :: endl;
        std :: cout << entry.address << std :: endl;
        std :: cout << entry.publickey << std :: endl;
    }).except([](const std :: exception_ptr &)
    {
        std :: cout << "Exception!" << std :: endl;
    });

    sleep(10_h);
}
