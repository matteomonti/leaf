#include <iostream>
#include <thread>

#include "drop/thread/semaphore.h"
#include "drop/thread/channel.hpp"
#include "drop/chrono/time.hpp"

using namespace drop;

channel <uint64_t> my_channel;
semaphore my_semaphore(100);

void f()
{
    uint64_t expected = 0;

    while(true)
    {
        optional <uint64_t> value = my_channel.pop();

        if(value)
        {
            if((*value) != expected)
            {
                std :: cout << "Unexpected value: " << (*value) << " vs " << expected << std :: endl;
                std :: terminate();
            }

            if((*value) % 1000000 == 0)
            {
                std :: cout << (*value) << std :: endl;
                my_semaphore.post();
            }

            expected++;
        }
    }
}

int main()
{
    std :: thread my_thread(f);

    for(uint64_t i = 0;; i++)
    {
        if(i % 1000000 == 0)
            my_semaphore.wait();

        my_channel.push(i);
    }
}
