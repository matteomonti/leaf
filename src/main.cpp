#include <iostream>
#include <thread>

// Includes

#include "drop/network/queue.h"
#include "drop/thread/wakepipe.h"

using namespace drop;

wakepipe my_wakepipe;
queue my_queue;

void thread()
{
    my_queue.add <queue :: read> (my_wakepipe);
    while(true)
    {
        my_queue.select();
        my_wakepipe.flush();

        std :: cout << "Here I am!" << std :: endl;
    }
}

int main()
{
    std :: thread my_thread(thread);

    while(true)
    {
        sleep(1_s);
        my_wakepipe.wake();
    }
}
