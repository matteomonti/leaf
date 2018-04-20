#include <iostream>

#include "poseidon/benchmark/coordinator.h"
#include "drop/crypto/signature.hpp"

using namespace drop;
using namespace vine;
using namespace poseidon;

struct ports
{
    static constexpr uint16_t coordinator = 7777;
    static constexpr uint16_t directory = 7778;
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
        sleep(10_h);
    }
    else if(!strcmp(args[1], "peer"))
    {
        if(argc < 3)
        {
            std :: cout << "Please insert the IP address of the master." << std :: endl;
            return -1;
        }

        signer signer;

        address coordaddr(args[2], ports :: coordinator);
        address diraddr(args[2], ports :: directory);

        std :: array <identifier, brahms :: settings :: view :: size> view = coordinator :: await(coordaddr, signer.publickey());

        for(size_t i = 0; i < brahms :: settings :: view :: size; i++)
            std :: cout << view[i] << std :: endl;
    }
}
