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

void sample(size_t node, signer * signers, identifier * sample)
{
    for(size_t i = 0; i < brahms :: settings :: view :: size; i++)
        sample[i] = signers[(node + 1 + (rand() % (nodes - 1))) % nodes].publickey();
}

int main()
{
    std :: ofstream nullstream;
    nullstream.open("/dev/null", std :: ios :: out);

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    /*std :: cout << "Starting server" << std :: endl;
    dialers :: directory :: server directory(7777);*/

    dialers :: local :: server server;

    sleep(0.1_s);

    signer signers[nodes];

    std :: cout << "Starting nodes" << std :: endl;

    brahms * brahms[nodes];
    for(size_t i = 0; i < nodes; i++)
    {
        signature :: publickey view[brahms :: settings :: view :: size];
        sample(i, signers, view);

        brahms[i] = new class brahms(signers[i], view, /*{"127.0.0.1", 7777}*/ server, connector, pool, crontab, (i == 0 ? std :: cout : nullstream));
    }

    std :: cout << "Started" << std :: endl;

    sleep(10_h);
}
