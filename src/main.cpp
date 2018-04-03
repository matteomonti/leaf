#include <iostream>
#include <fstream>

#include "poseidon/brahms/brahms.h"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "vine/dialers/local.h"

using namespace drop;
using namespace vine;
using namespace poseidon;

static constexpr size_t nodes = 64;

std :: array <identifier, brahms :: settings :: view :: size> view(std :: array <signer, nodes> & signers, size_t node)
{
    std :: array <identifier, brahms :: settings :: view :: size> sample;

    for(size_t i = 0; i < brahms :: settings :: view :: size; i++)
        sample[i] = signers[(node + 1 + (rand() % (nodes - 1))) % nodes].publickey();

    return sample;
}

int main()
{
    std :: ofstream nullstream;
    nullstream.open("/dev/null", std :: ios :: out);

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    dialers :: local :: server server;

    std :: array <signer, nodes> signers;
    std :: array <dialers :: local :: client *, nodes> dialers;
    std :: array <brahms *, nodes> brahms;

    std :: cout << "Creating nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        auto view = :: view(signers, i);
        dialers[i] = new dialers :: local :: client(server, signers[i]);

        brahms[i] = new class brahms(signers[i], view, *(dialers[i]), pool, crontab, (i == 0 ? std :: cout : nullstream));
    }

    brahms[0]->on <events :: push :: send> ([](const vine :: identifier & identifier)
    {
        std :: cout << "Going to push to " << identifier << std :: endl;

        //std :: cout << "Changed my mind!" << std :: endl;
        //return false;
    });

    brahms[0]->on <events :: push :: receive> ([](const vine :: identifier & identifier)
    {
        std :: cout << "Received push from " << identifier << std :: endl;

        std :: cout << "Changed my mind!" << std :: endl;
        return false;
    });

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
        brahms[i]->start();

    std :: cout << "Started" << std :: endl;

    sleep(10_h);
}
