#include <iostream>

#include "drop/distributed/publisher.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    pool pool;
    crontab crontab;

    sockets :: socketpair socketpair;

    publisher <uint64_t, buffer> publisher(crontab);
    subscriber <uint64_t, buffer> subscriber(pool.bind(socketpair.alpha), crontab);
}
