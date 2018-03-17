// Includes

#include "directory.h"

namespace poseidon
{
    // Signatures

    const buffer directory :: signatures :: entry = "directory :: signatures :: entry";

    // server

    // Settings

    constexpr uint16_t directory :: server :: settings :: port;
    constexpr interval directory :: server :: settings :: timeout;

    // Constructors

    directory :: server :: server() : _last_swap(now), _acceptor(settings :: port)
    {
        this->_acceptor.on <connection> ([=](const connection & connection)
        {
            this->serve(this->_pool.bind(connection));
        });
    }

    // Private methods

    promise <void> directory :: server :: serve(pool :: connection connection)
    {
        try
        {
            bool set = co_await connection.receive <bool> ();

            signature :: publickey identifier = co_await connection.receive <signature :: publickey> ();

            if(set)
            {
                entry entry;
                entry.address = connection.remote();
                entry.publickey = co_await connection.receive <class keyexchanger :: publickey> ();
                entry.signature = co_await connection.receive <signature> ();

                verifier verifier(identifier);
                verifier.verify(entry.signature, signatures :: entry, entry.publickey);

                this->_front[identifier] = entry;
            }
            else
            {
                if((timestamp(now) - this->_last_swap) > settings :: timeout)
                {
                    this->_last_swap = now;
                    std :: swap(this->_front, this->_back);
                    this->_front.clear();
                }

                optional <entry> entry;

                try
                {
                    entry = this->_front.at(identifier);
                }
                catch(...)
                {
                    try
                    {
                        entry = this->_back.at(identifier);
                    }
                    catch(...)
                    {
                    }
                }

                co_await connection.send((bool) entry);

                if(entry)
                {
                    co_await connection.send(entry->address);
                    co_await connection.send(entry->publickey);
                    co_await connection.send(entry->signature);
                }
            }
        }
        catch(...)
        {
        }
    }
};
