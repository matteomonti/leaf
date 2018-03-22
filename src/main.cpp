#include <iostream>
#include <unordered_map>

#include "poseidon/network/directory.h"

using namespace drop;
using namespace poseidon;

int main()
{
    directory :: server server(7777);

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    directory :: client client({"127.0.0.1", 7777}, connector, pool, crontab);

    sleep(10_h);
}
