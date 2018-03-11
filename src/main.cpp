#include <iostream>
#include <thread>

#include "drop/network/queue.h"
#include "drop/network/sockets/tcp.hpp"

using namespace drop;

int main()
{
    sockets :: tcp my_socket;
    my_socket.connect({"127.0.0.1", 1234});

    queue my_queue;
    my_queue.add <queue :: read> (my_socket.descriptor());

    std :: cout << "Selecting!" << std :: endl;
    size_t selected = my_queue.select();
    std :: cout << "Selected " << selected << std :: endl;

    std :: cout << my_queue[0].type() << std :: endl;
    std :: cout << my_queue[0].descriptor() << std :: endl;
}
