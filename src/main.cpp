#include <iostream>
#include <thread>

#include "drop/network/acceptors/tcp.h"

using namespace drop;

int main()
{
    acceptors :: tcp :: sync my_acceptor(1235);
    connection my_connection = my_acceptor.accept();
}
