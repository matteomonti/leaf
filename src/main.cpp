#include <iostream>
#include <thread>

#include "drop/thread/semaphore.h"

using namespace drop;

semaphore my_semaphore;

void f()
{
    std :: cout << "Waiting for the semaphore.." << std :: endl;

    if(my_semaphore.wait(timestamp(now) + 3_s))
        std :: cout << "Done (true)!" << std :: endl;
    else
        std :: cout << "Done (false)!" << std :: endl;
}

int main()
{
    std :: cout << "Launching thread..." << std :: endl;
    std :: thread my_thread(f);

    sleep(5_s);
    my_semaphore.post();

    my_thread.join();
    std :: cout << "Good night" << std :: endl;
}
