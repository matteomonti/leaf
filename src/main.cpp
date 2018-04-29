#include <iostream>
#include <fstream>

#include "poseidon/brahms/brahms.h"
#include "poseidon/poseidon/gossiper.h"

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
    poseidon :: gossiper * gossipers[nodes];

    for(size_t i = 0; i < nodes; i++)
    {
        brahms[i] = new class brahms(signers[i], views[i], *(dialers[i]), pool, crontab);
        gossipers[i] = new poseidon :: gossiper(signers[i], *(brahms[i]), *(dialers[i]), pool, crontab);
    }

    // Experiment

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        brahms[i]->start();
        gossipers[i]->start();
        sleep(0.1_s);
    }

    gossipers[0]->on <statement> ([](const statement & statement)
    {
        std :: cout << "Received statement: " << statement.identifier() << " / " << statement.sequence() << ": " << statement.value() << std :: endl;
    });

    for(size_t i = 0;; i++)
    {
        gossipers[44]->publish({signers[44], i, "I love apples!"});
        sleep(1_s);
    }
}
