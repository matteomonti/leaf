#include <iostream>
#include <fstream>

#include "poseidon/poseidon/poseidon.h"

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

    // Poseidon

    std :: cout << "Creating nodes" << std :: endl;

    class poseidon * peers[nodes];

    for(size_t i = 0; i < nodes; i++)
        peers[i] = new class poseidon(signers[i], views[i], *(dialers[i]), pool, crontab, ((i == 0) ? std :: cout : mute));

    // Experiment

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        peers[i]->start();
        sleep(0.1_s);
    }

    std :: cout << "All started" << std :: endl;

    sleep(10_s);

    for(uint64_t sequence = 0;; sequence++)
    {
        peers[44]->publish(sequence, "I love apples!");
        sleep(1_s);
    }

    sleep(10_h);
}
