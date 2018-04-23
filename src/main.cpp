#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    pool pool;

    gossiper <uint64_t> alice(crontab);
    gossiper <uint64_t> bob(crontab);

    bob.on <uint64_t> ([](const uint64_t & value)
    {
        std :: cout << "Bob receives " << value << std :: endl;
    });

    sockets :: socketpair socketpair;

    auto alicehandle = alice.serve(pool.bind(socketpair.alpha), true);
    auto bobhandle = bob.serve(pool.bind(socketpair.beta), false);

    for(uint64_t i = 0;; i++)
    {
        alice.publish(i);
        std :: cout << "Alice is " << (alicehandle.alive() ? "alive" : "dead") << std :: endl;
        sleep(0.1_s);
    }
}
