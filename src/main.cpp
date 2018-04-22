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

    acceptor.on <connection> ([&](const connection & connection) -> promise <void>
    {
        messenger <uint64_t, buffer> bob(pool.bind(connection), crontab);

        bob.on <uint64_t> ([](const uint64_t & value)
        {
            std :: cout << "[bob] Received value: " << value << std :: endl;
        });

        bob.on <buffer> ([](const buffer & value)
        {
            std :: cout << "[bob] Received value: " << value << std :: endl;
        });

        bob.on <drop :: close> ([]()
        {
            std :: cout << "[bob] Connection closing!" << std :: endl;
        });

        co_await crontab.wait(10_h);
    });

    connection connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
    sleep(0.1_s);

    {
        messenger <uint64_t, buffer> alice(pool.bind(connection), crontab);

        alice.on <drop :: close> ([]()
        {
            std :: cout << "[alice] Connection closing!" << std :: endl;
        });

        sleep(17_s);
        for(uint64_t i = 0; i < 10; i++)
        {
            if(i % 2)
                alice.send(i);
            else
                alice.send(buffer("I like apples!"));

            sleep(1_s);
        }
    }
    sleep(10_h);
}
