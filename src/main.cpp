#include <iostream>

#include "poseidon/benchmark/staticsample/master.h"
#include "poseidon/benchmark/staticsample/peer.h"

using namespace drop;
using namespace vine;
using namespace poseidon;

struct ports
{
    static constexpr uint16_t coordinator = 7777;
    static constexpr uint16_t directory = 7778;
};

struct sizes
{
    static constexpr size_t view = 4;
    static constexpr size_t sample = 8;
};

static constexpr interval rate = 5_s;

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

        staticsample :: master master(ports :: coordinator, ports :: directory, nodes);

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

        std :: vector <identifier> view = coordinator :: await(coordaddr, signer.publickey(), sizes :: view);
        std :: vector <identifier> sample = coordinator :: await(coordaddr, signer.publickey(), sizes :: sample);

        staticsample :: peer peer(instanceid, signer, rate, view, sample, diraddr);
        peer.start();
        
        while(true)
            sleep(10_h);
    }
}
