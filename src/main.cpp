#include <iostream>

#include "drop/network/sockets/tcp.h"

using namespace drop;

int main()
{
    sockets :: tcp my_socket;
    my_socket.connect({"127.0.0.1", 1234});

    while(true)
    {
        std :: cout << my_socket.available() << ", " << my_socket.space() << std :: endl;
        sleep(1_s);
    }
}
