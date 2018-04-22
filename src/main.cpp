#include <iostream>

#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.h"
#include "drop/distributed/messenger.hpp"

using namespace drop;

int main()
{
    acceptors :: tcp :: async acceptor(1234);
    crontab crontab;
    pool pool;

    acceptor.on <connection> ([&](const connection & connection)
    {
        messenger <uint64_t> my_messenger(pool.bind(connection), crontab);
    });

    connection connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});

    sleep(10_h);
}
