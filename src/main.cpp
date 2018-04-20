#include <iostream>
#include <fstream>

#include "poseidon/benchmark/coordinator.h"
#include "drop/crypto/signature.hpp"
#include "poseidon/poseidon/poseidon.h"
#include "vine/dialers/directory.h"

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
    static constexpr interval initial = 2_m;

    static constexpr interval total = 5_m;
    static constexpr interval publish = 5_s;
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

        std :: ofstream mute;
        mute.open("/dev/null", std :: ios :: out);

        address coordaddr(args[2], ports :: coordinator);
        address diraddr(args[2], ports :: directory);

        signer signer;
        std :: array <identifier, brahms :: settings :: view :: size> view = coordinator :: await(coordaddr, signer.publickey());

        connectors :: tcp :: async connector;
        pool pool;
        crontab crontab;

        multiplexer <dialers :: directory :: client, 3> dialer(diraddr, signer, connector, pool, crontab);

        class poseidon poseidon(signer, view, dialer, pool, crontab, std :: cout);

        poseidon.on <events :: gossip> ([](const statement & statement)
        {
            std :: cout << (uint64_t) timestamp(now) << " gossip " << statement.identifier() << " " << statement.sequence() << " " << statement.value() << std :: endl;
        });

        poseidon.on <events :: accept> ([](const statement & statement)
        {
            std :: cout << (uint64_t) timestamp(now) << " accept " << statement.identifier() << " " << statement.sequence() << " " << statement.value() << std :: endl;
        });

        std :: cout << brahms :: settings :: view :: size << " " << brahms :: settings :: sample :: size << " " << (decltype(poseidon) :: settings :: accept :: threshold) << std :: endl;
        poseidon.start();

        sleep(intervals :: initial);

        timestamp start = now;
        for(uint64_t round = 0; (timestamp(now) - start) < intervals :: total; round++)
        {
            if(instanceid == 0)
            {
                char message[1024];
                sprintf(message, "message-%llu", round);

                std :: cout << (uint64_t) timestamp(now) << " seed " << signer.publickey() << " " << round << " " << message << std :: endl;
                poseidon.publish(round, message);
            }

            sleep(intervals :: publish);
        }
    }
}
