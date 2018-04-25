#include <iostream>

#include "drop/distributed/publisher.hpp"
#include "drop/network/sockets/local.hpp"

using namespace drop;

int main()
{
    pool pool;
    crontab crontab;

    publisher <uint64_t, buffer> mypublisher(crontab);

    [&]() -> promise <void>
    {
        while(true)
        {
            co_await crontab.wait(1_s);
            mypublisher.publish(44, "Hello World!");
        }
    }();

    [&]() -> promise <void>
    {
        while(true)
        {
            co_await crontab.wait(0.5_s);
            mypublisher.publish(45, "World 45 is better!");
        }
    }();

    [&]() -> promise <void>
    {
        while(true)
        {
            co_await crontab.wait(0.1_s);
            mypublisher.publish(46, "I'm a crazy spammer!");
        }
    }();

    mypublisher.on <publisher <uint64_t, buffer> :: archive> ([](const auto & topic, const auto & archive)
    {
        std :: cout << "[archive] Request for archive on topic " << topic << std :: endl;

        archive << "Hello from the Archive!";
        archive << "Super hello from the Archive!";
        archive << "Duper hello from the Archive!";
    });

    sockets :: socketpair socketpair;
    mypublisher.serve(pool.bind(socketpair.beta));

    sockets :: socketpair othersocketpair;
    mypublisher.serve(pool.bind(othersocketpair.beta));

    subscriber <uint64_t, buffer> myothersubscriber(pool.bind(othersocketpair.alpha), crontab);

    size_t n = 0;
    myothersubscriber.on <buffer> ([&](const uint64_t & topic, const buffer & payload)
    {
        std :: cout << "Other subscriber receives: " << topic << ", " << payload << std :: endl;
        if(n++ >= 100)
            myothersubscriber.unsubscribe(topic);
    });

    myothersubscriber.on <drop :: close> ([]()
    {
        std :: cout << "Other subscriber closing" << std :: endl;
    });

    myothersubscriber.start();

    myothersubscriber.subscribe(46);

    {
        subscriber <uint64_t, buffer> mysubscriber(pool.bind(socketpair.alpha), crontab);

        mysubscriber.on <buffer> ([](const uint64_t & topic, const buffer & payload)
        {
            std :: cout << "Subscriber receives: " << topic << ", " << payload << std :: endl;
        });

        mysubscriber.on <drop :: close> ([]()
        {
            std :: cout << "Subscriber closing" << std :: endl;
        });

        mysubscriber.start();

        mysubscriber.subscribe(44);
        sleep(4_s);
        mysubscriber.unsubscribe(44);
        sleep(4_s);
        mysubscriber.once(46);
        sleep(4_s);
        mysubscriber.subscribe(45);
        sleep(4_s);
        mysubscriber.subscribe(44);
        sleep(10_s);
    }

    sleep(10_h);
}
