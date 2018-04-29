#include <iostream>

#include "poseidon/brahms/brahms.h"
#include "poseidon/poseidon/gossiper.h"
#include "poseidon/benchmark/coordinator.h"

using namespace drop;
using namespace vine;
using namespace poseidon;

struct ports
{
    static constexpr uint16_t coordinator = 7777;
    static constexpr uint16_t directory = 7778;
};

static constexpr interval rate = 1_s;

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

        while(true)
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

        std :: vector <identifier> viewvec = coordinator :: await(coordaddr, signer.publickey(), settings :: view :: size);

        view view;
        for(size_t i = 0; i < settings :: view :: size; i++)
            view[i] = viewvec[i];

        pool pool;
        crontab crontab;
        connectors :: tcp :: async connector;

        multiplexer <dialers :: directory :: client, settings :: channels> dialer(diraddr, signer, connector, pool, crontab);

        class brahms brahms(signer, view, dialer, pool, crontab);
        poseidon :: gossiper gossiper(signer, brahms, dialer, pool, crontab, std :: cout);
        brahms.start();
        gossiper.start();

        while(true)
            sleep(10_h);
    }
}
