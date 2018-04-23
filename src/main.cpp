#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    pool pool;

    gossiper <uint64_t> alice(crontab);

    alice.on <uint64_t> ([](const auto & id, const uint64_t & value)
    {
        std :: cout << "Alice receives " << value << std :: endl;
    });

    gossiper <uint64_t> bob(crontab);

    bob.on <uint64_t> ([&](const auto & id, const uint64_t & value)
    {
        std :: cout << "Bob receives " << value << std :: endl;
    });

    sockets :: socketpair socketpair;

    alice.serve(pool.bind(socketpair.alpha), true).until(timestamp(now) + 10_s).then([]()
    {
        std :: cout << "Bob drops the handle to Alice" << std :: endl;
    });

    bob.serve(pool.bind(socketpair.beta), false).then([]()
    {
        std :: cout << "Alice drops the handle to Bob" << std :: endl;
    });

    for(uint64_t i = 0;; i++)
    {
        alice.publish(i);
        sleep(0.1_s);
    }
}
