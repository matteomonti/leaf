#include <iostream>
#include <fstream>

#include "poseidon/brahms/brahms.h"

using namespace drop;
using namespace vine;
using namespace poseidon;

static constexpr size_t nodes = 64;

int main()
{
    // Mute

    std :: ofstream mute;
    mute.open("/dev/null", std :: ios :: out);

    // Runners

    pool pool;
    crontab crontab;

    // Signers

    signer signers[nodes];

    // Dialers

    dialers :: local :: server server;

    multiplexer <dialers :: local :: client, settings :: channels> * dialers[nodes];
    for(size_t i = 0; i < nodes; i++)
        dialers[i] = new multiplexer <dialers :: local :: client, settings :: channels> (server, signers[i], pool);

    // Views

    view views[nodes];

    for(size_t i = 0; i < nodes; i++)
        for(size_t j = 0; j < settings :: view :: size; j++)
            views[i][j] = signers[randombytes_uniform(nodes)].publickey();

    // Brahms

    std :: cout << "Creating nodes" << std :: endl;

    brahms * brahms[nodes];

    for(size_t i = 0; i < nodes; i++)
        brahms[i] = new class brahms(signers[i], views[i], *(dialers[i]), pool, crontab);

    // Experiment

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        brahms[i]->start();
        sleep(0.1_s);
    }

    while(true)
    {
        size_t index;
        std :: cin >> index;

        identifier identifier = (*(brahms[0]))[index];
        std :: cout << "brahms[" << index << "] = " << identifier << std :: endl;
        std :: cout << "ban?" << std :: endl;

        char ban;
        std :: cin >> ban;

        if(ban == 'y')
            brahms[0]->ban(identifier);
    }
}
