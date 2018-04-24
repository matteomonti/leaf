#include <iostream>

#include "drop/distributed/publisher.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    publisher <uint64_t, buffer> publisher(crontab);

    publisher.subscribe(11, 14, false);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.subscribe(11, 15, false);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.subscribe(12, 14, true);

    std :: cout << "=====================================" << std :: endl;

    publisher.unsubscribe(99, 100, true);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.unsubscribe(11, 14, false);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.unsubscribe(11, 15, false);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.unsubscribe(12, 14, true);
}
