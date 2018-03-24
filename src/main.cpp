#include <iostream>

#include "poseidon/brahms/brahms.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "poseidon/network/directory.hpp"

using namespace drop;
using namespace poseidon;

static constexpr size_t nodes = 256;

void sample(signer * signers, signature :: publickey * sample)
{
    for(size_t i = 0; i < brahms :: settings :: view :: size; i++)
        sample[i] = signers[rand() % nodes].publickey();
}

int main()
{
    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    std :: cout << "Starting server" << std :: endl;
    directory :: server directory(7777);

    sleep(0.1_s);

    signer signers[nodes];

    std :: cout << "Starting nodes" << std :: endl;

    brahms * brahms[nodes];
    for(size_t i = 0; i < nodes; i++)
    {
        signature :: publickey view[brahms :: settings :: view :: size];
        sample(signers, view);

        brahms[i] = new class brahms(signers[i], view, {"127.0.0.1", 7777}, connector, pool, crontab);
    }

    brahms[0]->log = true;

    std :: cout << "Started" << std :: endl;

    sleep(10_h);
}
