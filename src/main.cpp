#include <iostream>
#include <fstream>

#include "poseidon/brahms/brahms.h"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "vine/dialers/local.h"
#include "vine/network/multiplexer.hpp"

using namespace drop;
using namespace vine;
using namespace poseidon;

static constexpr size_t nodes = 256;

std :: array <identifier, brahms :: settings :: view :: size> view(std :: array <signer, nodes> & signers, size_t exclude)
{
    std :: array <identifier, brahms :: settings :: view :: size> sample;

    for(size_t i = 0; i < brahms :: settings :: view :: size; i++)
    {
        size_t pick = rand() % nodes;

        while(pick == exclude)
            pick = rand() % nodes;

        sample[i] = signers[pick].publickey();
    }

    return sample;
}

int main()
{
    std :: ofstream mute;
    mute.open("/dev/null", std :: ios :: out);

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    dialers :: local :: server server;

    std :: array <signer, nodes> signers;
    std :: array <multiplexer <dialers :: local :: client, 3> *, nodes> dialers;
    std :: array <class brahms *, nodes> brahms;

    std :: cout << "Creating nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        auto view = :: view(signers, i);
        dialers[i] = new multiplexer <dialers :: local :: client, 3> (server, signers[i], pool);
        brahms[i] = new class brahms(signers[i], view, *(dialers[i]), pool, crontab);
    }

    std :: cout << "Registering handlers" << std :: endl;

    brahms[0]->on <events :: push :: send> ([](const identifier & identifier)
    {
        std :: cout << "Sending push to " << identifier << std :: endl;
    });

    brahms[0]->on <events :: push :: receive> ([](const identifier & identifier)
    {
        std :: cout << "Received push from " << identifier << std :: endl;
    });

    brahms[0]->on <events :: view :: join> ([](const identifier & identifier)
    {
        std :: cout << "View join: " << identifier << std :: endl;
    });

    brahms[0]->on <events :: view :: leave> ([](const identifier & identifier)
    {
        std :: cout << "View leave: " << identifier << std :: endl;
    });

    brahms[0]->on <events :: sample :: join> ([](const identifier & identifier)
    {
        std :: cout << "Sample join: " << identifier << std :: endl;
    });

    brahms[0]->on <events :: sample :: leave> ([](const identifier & identifier)
    {
        std :: cout << "Sample leave: " << identifier << std :: endl;
    });

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        std :: cout << "Starting node " << i << std :: endl;
        brahms[i]->start();
        // sleep(30_ms);
    }

    std :: cout << "Started" << std :: endl;

    sleep(10_h);
}
