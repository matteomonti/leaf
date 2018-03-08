#include <iostream>

#include "drop/chrono/crontab.h"

using namespace drop;

std :: mutex mutex;

promise <void> f(crontab & crontab)
{
    while(true)
    {
        timestamp target = timestamp(now) + interval(rand() % 1000000 + 100000);
        co_await crontab.wait(target);
        timestamp actual = now;

        if(actual - target > 10_ms)
        {
            mutex.lock();
            std :: cout << "Significant delay: " << (actual - target) << std :: endl;
            mutex.unlock();
        }
    }
}

int main()
{
    crontab my_crontabs[8];

    for(size_t c = 0; c < 8; c++)
        for(size_t i = 0; i < 50000; i++)
            f(my_crontabs[c]);

    sleep(10_h);
}
