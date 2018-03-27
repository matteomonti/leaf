// Includes

#include "directory.hpp"

namespace vine :: dialers
{
    using namespace drop;

    // Signatures

    const hash directory :: signatures :: entry = hash(buffer("vine :: dialers :: directory :: signatures :: entry"));

    // server

    // Settings

    constexpr interval directory :: server :: settings :: timeout;

    // Constructors

    directory :: server :: server(const uint16_t & port) : _last_swap(now), _acceptor(port)
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

            identifier identifier = co_await connection.receive <vine :: identifier> ();

            if(set)
            {
                entry entry;

                entry.address = {connection.remote().ip(), co_await connection.receive <class address :: port> ()};
                entry.publickey = co_await connection.receive <class keyexchanger :: publickey> ();
                entry.timestamp = co_await connection.receive <timestamp> ();
                entry.signature = co_await connection.receive <signature> ();

                verifier verifier(identifier);
                verifier.verify(entry.signature, signatures :: entry, entry.publickey, entry.timestamp);

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

                bool valid = ((bool) entry) && (timestamp(now) - entry->timestamp < settings :: timeout);
                co_await connection.send(valid);

                if(valid)
                {
                    co_await connection.send(entry->address);
                    co_await connection.send(entry->publickey);
                    co_await connection.send(entry->timestamp);
                    co_await connection.send(entry->signature);
                }
            }
        }
        catch(...)
        {
        }
    }
};
