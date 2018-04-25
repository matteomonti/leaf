#include <iostream>

#include "drop/distributed/gossiper.hpp"
#include "drop/distributed/publisher.hpp"
#include "poseidon/benchmark/coordinator.h"
#include "drop/crypto/signature.hpp"
#include "vine/dialers/directory.hpp"
#include "vine/network/multiplexer.hpp"

using namespace drop;
using namespace vine;
using namespace poseidon;

struct ports
{
    static constexpr uint16_t coordinator = 7777;
    static constexpr uint16_t directory = 7778;
};

struct intervals
{
    static constexpr interval initial = 10_s;
    static constexpr interval final = 30_s;

    static constexpr interval total = 3_m;
    static constexpr interval publish = 5_s;
    static constexpr interval change = 3_m;
};

// Statement

struct statement
{
    // Public members

    uint64_t id;
    buffer message;

    // Methods

    template <typename vtype> void accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->id);
        reader << (this->message);
    }

    template <typename vtype> void accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->id);
        writer >> (this->message);
    }

    // Operators

    bool operator == (const statement & rho) const
    {
        return (this->id == rho.id) && (this->message == rho.message);
    }

    bool operator != (const statement & rho) const
    {
        return !((*this) == rho);
    }
};

int main(int argc, char ** args)
{
    if(argc < 2)
    {
        std :: cout << "Please select a role for the node: master or peer." << std :: endl;
        return -1;
    }

    if(!strcmp(args[1], "master"))
    {
        if(argc < 3)
        {
            std :: cout << "Please insert the number of nodes to coordinate." << std :: endl;
            return -1;
        }

        size_t nodes = atoi(args[2]);

        if(nodes == 0)
        {
            std :: cout << "The network cannot have zero nodes." << std :: endl;
            return -1;
        }

        coordinator coordinator(ports :: coordinator, nodes);
        dialers :: directory :: server directory(ports :: directory);
        sleep(10_h);
    }
    else if(!strcmp(args[1], "peer"))
    {
        if(argc < 3)
        {
            std :: cout << "Please insert the IP address of the master." << std :: endl;
            return -1;
        }

        if(argc < 4)
        {
            std :: cout << "Please insert the ID of the current instance." << std :: endl;
            return -1;
        }

        size_t instanceid = atoi(args[3]);

        address coordaddr(args[2], ports :: coordinator);
        address diraddr(args[2], ports :: directory);

        // Coordinator

        signer signer;

        std :: cout << "Node publickey: " << signer.publickey() << std :: endl;

        std :: array <identifier, coordinator :: settings :: view :: size> view = coordinator :: await(coordaddr, signer.publickey());
        std :: array <identifier, coordinator :: settings :: view :: size> sample = coordinator :: await(coordaddr, signer.publickey());

        // Various declarations

        connectors :: tcp :: async connector;
        pool pool;
        crontab crontab;

        // Publisher

        std :: mutex mutex;

        std :: unordered_map <uint64_t, buffer> statements;
        std :: unordered_map <uint64_t, size_t> votes;

        publisher <uint64_t, buffer> ppublisher(crontab);

        ppublisher.on <publisher <uint64_t, buffer> :: archive> ([&](const auto & id, const auto & archive)
        {
            mutex.lock();

            try
            {
                buffer message = statements.at(id);
                archive << message;
            }
            catch(...)
            {
            }

            mutex.unlock();
        });

        subscriber <uint64_t, buffer> * subscribers[coordinator :: settings :: view :: size]{};

        // Gossiper

        gossiper <statement> gossiper(crontab);

        gossiper.on <statement> ([&](const auto & id, const statement & statement)
        {
            std :: cout << (uint64_t) timestamp(now) << " gossip " << statement.id << ": " << statement.message << std :: endl;

            mutex.lock();
            statements[statement.id] = statement.message;
            votes[statement.id] = 0;

            for(size_t i = 0; i < coordinator :: settings :: view :: size; i++)
                if(subscribers[i])
                    subscribers[i]->once(statement.id);

            mutex.unlock();

            ppublisher.publish(statement.id, statement.message);
        });

        // Dialer

        multiplexer <dialers :: directory :: client, 2> dialer(diraddr, signer, connector, pool, crontab);

        dialer.on <0> ([&](const dial & dial)
        {
            gossiper.serve(pool.bind(dial), signer.publickey() < dial.identifier());
        });

        dialer.on <1> ([&](const dial & dial)
        {
            ppublisher.serve(pool.bind(dial));
        });

        // Seeding

        timestamp start = now;

        if(instanceid == 0)
        {
            [&]() -> promise <void>
            {
                for(uint64_t round = 0; (timestamp(now) - start) < intervals :: total; round++)
                {
                    std :: cout << (uint64_t) timestamp(now) << " seed " << round << std :: endl;

                    char message[1024];
                    sprintf(message, "Message number %llu", round);
                    gossiper.publish({.id = round, .message = message});

                    co_await crontab.wait(intervals :: publish);
                }
            }();
        }

        sleep(intervals :: initial);

        // Experiment

        while(true)
        {
            if(timestamp(now) - start > intervals :: total + intervals :: final)
                break;

            mutex.lock();
            for(size_t i = 0; i < coordinator :: settings :: view :: size; i++)
                if(subscribers[i])
                {
                    delete subscribers[i];
                    subscribers[i] = nullptr;
                }

            std :: cout << (uint64_t) timestamp(now) << " reset" << std :: endl;
            for(auto & vote : votes)
                vote.second = 0;

            mutex.unlock();

            for(size_t index = 0; index < coordinator :: settings :: view :: size; index++)
            {
                identifier identifier = sample[index];

                dialer.connect <1> (identifier).then([&, index](const dial & dial)
                {
                    mutex.lock();

                    subscribers[index] = new subscriber <uint64_t, buffer> (pool.bind(dial), crontab);
                    subscribers[index]->on <buffer> ([&](const uint64_t & id, const buffer & message)
                    {
                        mutex.lock();
                        votes[id]++;
                        std :: cout << (uint64_t) timestamp(now) << " votes " << id << " " << votes[id] << std :: endl;

                        if(votes[id] == coordinator :: settings :: view :: size)
                        {
                            std :: cout << (uint64_t) timestamp(now) << " accept " << id << ": " << message << std :: endl;
                            votes.erase(id);
                        }

                        mutex.unlock();
                    });

                    subscribers[index]->on <drop :: close> ([=]()
                    {
                        std :: cout << "Subscriber " << index << "lost the connection" << std :: endl;
                    });

                    for(const auto & vote : votes)
                        subscribers[index]->once(vote.first);

                    subscribers[index]->start();

                    mutex.unlock();
                }).except([=](const std :: exception_ptr & exception)
                {
                    std :: cout << "Failed to establish (sample) connection to " << identifier << std :: endl;
                });
            }

            for(const identifier & identifier : view)
                dialer.connect <0> (identifier).then([&](const dial & dial)
                {
                    gossiper.serve(pool.bind(dial), signer.publickey() < dial.identifier()).until(timestamp(now) + intervals :: change /*interval :: random(intervals :: change)*/);
                }).except([=](const std :: exception_ptr & exception)
                {
                    std :: cout << "Failed to establish (view) connection to " << identifier << std :: endl;
                });

            sleep(intervals :: change/*interval :: random(intervals :: change)*/);

            break; // REMOVE ME
            view = coordinator :: await(coordaddr, signer.publickey());
            sample = coordinator :: await(coordaddr, signer.publickey());
        }

        std :: cout << "Main thread terminating" << std :: endl;
    }
}
