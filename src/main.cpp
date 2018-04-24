#include <iostream>

#include "drop/distributed/publisher.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    publisher <uint64_t, buffer> publisher(crontab);
}
