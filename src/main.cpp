#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    pool pool;

    gossiper <uint64_t> alice(crontab);
    alice.on <uint64_t> ([](const uint64_t & message)
    {
        std :: cout << "[alice] Received: " << message << std :: endl;
    });

    gossiper <uint64_t> bob(crontab);
    bob.on <uint64_t> ([](const uint64_t & message)
    {
        std :: cout << "[bob] Received: " << message << std :: endl;
    });

    gossiper <uint64_t> carl(crontab);
    carl.on <uint64_t> ([](const uint64_t & message)
    {
        std :: cout << "[carl] Received: " << message << std :: endl;
    });

    gossiper <uint64_t> dave(crontab);
    dave.on <uint64_t> ([](const uint64_t & message)
    {
        std :: cout << "[dave] Received: " << message << std :: endl;
    });


    alice.publish(1000);
    alice.publish(1001);
    alice.publish(1002);

    {
        sockets :: socketpair socketpair;
        alice.serve(pool.bind(connection(socketpair.alpha)), true);
        bob.serve(pool.bind(connection(socketpair.beta)), false);
    }

    sleep(2_s);

    for(uint64_t i = 0; i < 5; i++)
    {
        bob.publish(i);
        sleep(1_s);
    }

    {
        sockets :: socketpair socketpair;
        alice.serve(pool.bind(connection(socketpair.alpha)), true);
        carl.serve(pool.bind(connection(socketpair.beta)), false);
    }

    sleep(2_s);

    for(uint64_t i = 5; i < 10; i++)
    {
        bob.publish(i);
        sleep(1_s);
    }

    {
        sockets :: socketpair socketpair;
        dave.serve(pool.bind(connection(socketpair.alpha)), true);
        alice.serve(pool.bind(connection(socketpair.beta)), false);
    }

    {
        sockets :: socketpair socketpair;
        dave.serve(pool.bind(connection(socketpair.alpha)), true);
        bob.serve(pool.bind(connection(socketpair.beta)), false);
    }

    {
        sockets :: socketpair socketpair;
        dave.serve(pool.bind(connection(socketpair.alpha)), true);
        carl.serve(pool.bind(connection(socketpair.beta)), false);
    }

    sleep(2_s);

    for(uint64_t i = 10; i < 15; i++)
    {
        dave.publish(i);
        sleep(1_s);
    }

    sleep(10_h);
}
