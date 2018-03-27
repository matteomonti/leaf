#include <iostream>

#include "vine/dialers/directory.hpp"

using namespace drop;
using namespace vine;

int main()
{
    dialers :: directory :: server server(7777);
    std :: cout << "Server started" << std :: endl;

    connectors :: tcp :: async connector;
    pool pool;
    crontab crontab;

    dialers :: directory :: client alice({"127.0.0.1", 7777}, connector, pool, crontab);
    dialers :: directory :: client bob({"127.0.0.1", 7777}, connector, pool, crontab);

    /*alice.on <dial> ([](const dial & dial)
    {
        std :: cout << "[alice] Connection incoming from " << dial.identifier() << std :: endl;
    });

    std :: cout << "[alice] Dial handler registered" << std :: endl;*/

    sleep(1_s);

    bob.connect(alice.identifier()).then([](const dial & dial)
    {
        std :: cout << "[bob] Connection established with " << dial.identifier() << std :: endl;
    }).except([](const std :: exception_ptr & exception)
    {
        try
        {
            std :: rethrow_exception(exception);
        }
        catch(const std :: exception & exception)
        {
            std :: cout << "Exception: " << exception.what() << std :: endl;
        }
    });

    sleep(10_h);
}
