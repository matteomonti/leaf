#include <iostream>
#include <fstream>

#include "poseidon/poseidon/crawler.h"
#include "poseidon/poseidon/gossiper.h"
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
    std :: ofstream logs[nodes];
    for(size_t i = 0; i < nodes; i++)
    {
        char path[1024];
        sprintf(path, "/Users/monti/logs/%d.log", (int)i);
        std :: cout << "Trying to open " << path << std :: endl;
        logs[i].open(path, std :: ios :: out);
    }

    std :: ofstream mute;
    mute.open("/dev/null", std :: ios :: out);

    std :: cout << "Logs opened" << std :: endl;

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    dialers :: local :: server server;

    std :: array <signer, nodes> signers;
    std :: array <multiplexer <dialers :: local :: client, 3> *, nodes> dialers;
    std :: array <gossiper *, nodes> gossipers;
    std :: array <crawler *, nodes> crawlers;

    std :: cout << "Creating nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        auto view = :: view(signers, i);
        dialers[i] = new multiplexer <dialers :: local :: client, 3> (server, signers[i], pool);
        gossipers[i] = new gossiper(signers[i].publickey(), crontab, (i == 0 ? std :: cout : mute));

        crawlers[i] = new crawler(signers[i], view, *(gossipers[i]), *(dialers[i]), pool, crontab);
    }

    std :: cout << "Registering statement handlers" << std :: endl;

    size_t * spreading = new size_t(0);

    for(size_t i = 0; i < nodes; i++)
        gossipers[i]->on <statement> ([=](const statement & statement)
        {
            std :: cout << (*spreading) << ": Gossiper " << i << " received statement " << statement.identifier() << " / " << statement.sequence() << ": " << statement.value() << std :: endl;
            (*spreading)++;
        });

    std :: cout << "Seeding gossip" << std :: endl;

    statement seed(signers[44], 0, "I love apples!");
    gossipers[44]->add(seed);

    std :: cout << "Starting nodes" << std :: endl;

    for(size_t i = 0; i < nodes; i++)
    {
        std :: cout << "Starting node " << i << std :: endl;
        crawlers[i]->start();
        sleep(200_ms);
    }

    std :: cout << "Started" << std :: endl;
    sleep(10_h);
}
