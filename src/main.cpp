#include <iostream>

#include "drop/network/sockets/local.hpp"
#include "drop/network/connection.hpp"
#include "drop/network/pool.hpp"

using namespace drop;

int main()
{
    sockets :: socketpair my_socketpair;

    connection alpha = my_socketpair.alpha;
    connection beta = my_socketpair.beta;

    my_socketpair.alpha.send_timeout(10_s);
    my_socketpair.beta.receive_timeout(20_s);

    alpha.send(buffer("Hello World!"));
    std :: cout << beta.receive() << std :: endl;

    pool my_pool;

    pool :: connection async_alpha = my_pool.bind(alpha);
    pool :: connection async_beta = my_pool.bind(beta);

    async_alpha.send(buffer("Hello World!"));

    async_beta.receive().then([](const buffer & message)
    {
        std :: cout << message << std :: endl;
    });

    sleep(10_s);
}
