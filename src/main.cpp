#include <iostream>

#include "drop/network/sockets/udp.h"

using namespace drop;

int main()
{
    sockets :: udp my_socket;

    my_socket.bind(1234);

    auto packet = my_socket.receive();

    std :: cout << packet.remote() << std :: endl;
    std :: cout << packet.size() << std :: endl;
    std :: cout << packet.message() << std :: endl;
}
