#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    pool pool;

    gossiper <uint64_t> :: handle alicehandle, bobhandle;

    gossiper <uint64_t> alice(crontab);

    alice.on <uint64_t> ([](const auto & id, const uint64_t & value)
    {
        std :: cout << "Alice receives " << value << " (id is " << (id ? "non-null" : "null") << ")" << std :: endl;
    });

    gossiper <uint64_t> bob(crontab);

    bob.on <uint64_t> ([&](const auto & id, const uint64_t & value)
    {
        std :: cout << "Bob receives " << value << "(" << (id == alicehandle ? "it is" : "it is not") << " from Alice)" << std :: endl;
    });

    sockets :: socketpair socketpair;

    bobhandle = alice.serve(pool.bind(socketpair.alpha), true);
    alicehandle = bob.serve(pool.bind(socketpair.beta), false);

    for(uint64_t i = 0;; i++)
    {
        alice.publish(i);

        std :: cout << "Alice is " << (alicehandle.alive() ? "alive" : "dead") << std :: endl;
        std :: cout << "Bob is " << (bobhandle.alive() ? "alive" : "dead") << std :: endl;

        if(i == 20)
        {
            std :: cout << "Closing Alice's handle" << std :: endl;
            alicehandle.close();
        }

        sleep(0.1_s);
    }
}
