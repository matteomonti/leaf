// Includes

#include "directory.h"

namespace poseidon
{
    // Signatures

    const buffer directory :: signatures :: entry = "directory :: signatures :: entry";

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

            signature :: publickey identifier = co_await connection.receive <signature :: publickey> ();

            if(set)
            {
                entry entry;
                entry.address = {connection.remote().ip(), co_await connection.receive <class address :: port> ()};
                entry.publickey = co_await connection.receive <class keyexchanger :: publickey> ();
                entry.signature = co_await connection.receive <signature> ();

                verifier verifier(identifier);
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

    // client

    // Constructors

    directory :: client :: client(const address & server, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _server(server), _connector(connector), _pool(pool), _crontab(crontab)
    {
        this->refresh();
    }

    directory :: client :: client(const address & server, const class signer & signer, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _server(server),  _signer(signer), _connector(connector), _pool(pool), _crontab(crontab)
    {
        this->refresh();
    }

    directory :: client :: client(const address & server, const class signer & signer, const class keyexchanger & keyexchanger, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _server(server), _signer(signer), _keyexchanger(keyexchanger), _connector(connector), _pool(pool), _crontab(crontab)
    {
        this->refresh();
    }

    // Getters

    const signature :: publickey & directory :: client :: identifier() const
    {
        return this->_signer.publickey();
    }

    signer & directory :: client :: signer()
    {
        return this->_signer;
    }

    keyexchanger & directory :: client :: keyexchanger()
    {
        return this->_keyexchanger;
    }

    // Private methods

    promise <void> directory :: client :: refresh()
    {
        while(true)
        {
            pool :: connection connection = this->_pool.bind(co_await this->_connector.connect(this->_server));

            co_await connection.send(true);
            co_await connection.send(this->_signer.publickey());
            co_await connection.send(this->_acceptor.port());
            co_await connection.send(this->_keyexchanger.publickey());
            co_await connection.send(this->_signer.sign(signatures :: entry, this->_keyexchanger.publickey()));

            co_await this->_crontab.wait(settings :: refresh);
        }
    }
};
