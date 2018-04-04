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

    brahms[0]->on <events :: view :: join> ([](const identifier & identifier)
    {
        std :: cout << "Join: " << identifier << std :: endl;
    });

    brahms[0]->on <events :: view :: leave> ([](const identifier & identifier)
    {
        std :: cout << "Leave: " << identifier << std :: endl;
    });

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
        brahms[i]->start();

    std :: cout << "Started" << std :: endl;

    sleep(10_h);
}
