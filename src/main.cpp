#include <iostream>

#include "vine/dialers/local.h"
#include "vine/dialers/directory.h"
#include "vine/network/multiplexer.hpp"

using namespace drop;
using namespace vine;

template <typename... types, size_t value = sizeof...(types)> void f()
{
    std :: cout << value << std :: endl;
}

std :: mutex cmtx;

int main()
{
    pool pool;
    dialers :: local :: server server;

    multiplexer <dialers :: local :: client, 10> alice(server, pool);
    multiplexer <dialers :: local :: client, 10> bob(server, pool);

    bob.on <9> ([](const dial & dial)
    {
        cmtx.lock();
        std :: cout << "[bob] Dial received from " << dial.identifier() << std :: endl;
        cmtx.unlock();
    });

    alice.connect <9> (bob.identifier()).then([](const dial & dial)
    {
        cmtx.lock();
        std :: cout << "[alice] Connected" << std :: endl;
        cmtx.unlock();
    });
}
