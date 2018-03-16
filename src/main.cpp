#include <iostream>

#include "drop/crypto/keyexchanger.h"
#include "drop/network/connectors/tcp.h"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/pool.hpp"

using namespace drop;

std :: mutex cmtx;

keyexchanger serverkx;
keyexchanger clientkx;

promise <void> server(pool :: connection connection)
{
    co_await connection.authenticate(serverkx, clientkx.publickey());
    cmtx.lock(); std :: cout << "[server] Authentication successful" << std :: endl; cmtx.unlock();

    std :: cout << co_await connection.receive() << std :: endl;
}

promise <void> client(pool :: connection connection)
{
    co_await connection.authenticate(clientkx, serverkx.publickey());
    cmtx.lock(); std :: cout << "[client] Authentication successful" << std :: endl; cmtx.unlock();

    co_await connection.send(buffer("Hello World!"));
}

int main()
{
    pool pool;
    acceptors :: tcp :: sync acceptor(1234);

    connection client_connection = connectors :: tcp :: sync :: connect({"127.0.0.1", 1234});
    connection server_connection = acceptor.accept();

    server(pool.bind(server_connection));
    client(pool.bind(client_connection));

    sleep(10_h);
}
