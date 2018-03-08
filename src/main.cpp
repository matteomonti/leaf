#include <iostream>

#include "drop/chrono/crontab.h"

using namespace drop;

int main()
{
    crontab my_crontab;

    std :: cout << "Submitting" << std :: endl;
    my_crontab.wait(timestamp(now) - 2_ms).then([]()
    {
        std :: cout << "YESSSS!!" << std :: endl;
    });

    sleep(10_s);
}
