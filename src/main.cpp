#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "poseidon/benchmark/coordinator.h"
#include "drop/crypto/signature.hpp"
#include "vine/dialers/directory.hpp"

using namespace drop;
using namespace vine;
using namespace poseidon;

struct ports
{
    static constexpr uint16_t coordinator = 7777;
    static constexpr uint16_t directory = 7778;
};

struct intervals
{
    static constexpr interval initial = 10_s;

    static constexpr interval total = 30_s;
    static constexpr interval publish = 0.01_s;
};

int main(int argc, char ** args)
{
    if(argc < 2)
    {
        std :: cout << "Please select a role for the node: master or peer." << std :: endl;
        return -1;
    }

    if(!strcmp(args[1], "master"))
    {
        if(argc < 3)
        {
            std :: cout << "Please insert the number of nodes to coordinate." << std :: endl;
            return -1;
        }

        size_t nodes = atoi(args[2]);

        if(nodes == 0)
        {
            std :: cout << "The network cannot have zero nodes." << std :: endl;
            return -1;
        }

        coordinator coordinator(ports :: coordinator, nodes);
        dialers :: directory :: server directory(ports :: directory);
        sleep(10_h);
    }
    else if(!strcmp(args[1], "peer"))
    {
        if(argc < 3)
        {
            std :: cout << "Please insert the IP address of the master." << std :: endl;
            return -1;
        }

        if(argc < 4)
        {
            std :: cout << "Please insert the ID of the current instance." << std :: endl;
            return -1;
        }

        size_t instanceid = atoi(args[3]);

        address coordaddr(args[2], ports :: coordinator);
        address diraddr(args[2], ports :: directory);

        signer signer;
        std :: array <identifier, coordinator :: settings :: view :: size> view = coordinator :: await(coordaddr, signer.publickey());

        connectors :: tcp :: async connector;
        pool pool;
        crontab crontab;

        gossiper <uint64_t> gossiper(crontab);

        gossiper.on <uint64_t> ([](const uint64_t & value)
        {
            std :: cout << (uint64_t) timestamp(now) << " gossip " << value << std :: endl;
        });

        dialers :: directory :: client dialer(diraddr, signer, connector, pool, crontab);

        dialer.on <dial> ([&](const dial & dial)
        {
            gossiper.serve(pool.bind(dial), signer.publickey() < dial.identifier());
        });

        sleep(intervals :: initial);

        for(const identifier & identifier : view)
            dialer.connect(identifier).then([&](const dial & dial)
            {
                gossiper.serve(pool.bind(dial), signer.publickey() < dial.identifier());
            }).except([=](const std :: exception_ptr & exception)
            {
                std :: cout << "Failed to establish connection to " << identifier << std :: endl;
            });

        timestamp start = now;
        for(uint64_t round = 0; (timestamp(now) - start) < intervals :: total; round++)
        {
            if(instanceid == 0)
            {
                std :: cout << (uint64_t) timestamp(now) << " seed " << round << std :: endl;
                gossiper.publish(round);
            }

            sleep(intervals :: publish);
        }
    }
}
