#include <iostream>

#include "drop/distributed/publisher.hpp"

using namespace drop;

int main()
{
    crontab crontab;
    publisher <uint64_t, buffer> publisher(crontab);

    publisher.add({11, 14, false});
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.add({11, 15, false});
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.add({12, 14, true});

    std :: cout << "=====================================" << std :: endl;
    publisher.remove({99, 144, true});
    std :: cout << "=====================================" << std :: endl;
    
    publisher.clear(44);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.clear(15);
    std :: cout << "-------------------------------------" << std :: endl;
    publisher.clear(14);
}
