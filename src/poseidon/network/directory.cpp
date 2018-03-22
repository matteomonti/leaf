// Includes

#include "directory.hpp"

namespace poseidon
{
    // Signatures

    const buffer directory :: signatures :: entry = "directory :: signatures :: entry";

    // connection

    // Constructors

    directory :: connection :: connection(const :: drop :: connection & connection, const signature :: publickey & identifier) : :: drop :: connection(connection), _identifier(identifier)
    {
    }

    // Getters

    const signature :: publickey & directory :: connection :: identifier() const
    {
        return this->_identifier;
    }

    // server

    // Settings

    constexpr interval directory :: server :: settings :: timeout;

    // Constructors

    directory :: server :: server(const uint16_t & port) : _last_swap(now), _acceptor(port)
    {
        this->_acceptor.on <:: drop :: connection> ([=](const :: drop :: connection & connection)
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

    // client

    // Exceptions

    const char * directory :: client :: exceptions :: lookup_failed :: what() const throw()
    {
        return "Directory lookup failed.";
    }

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

    // Methods

    promise <directory :: connection> directory :: client :: connect(signature :: publickey identifier)
    {
        entry entry = co_await this->lookup(identifier);

        try
        {
            entry = this->_cache.at(identifier);
        }
        catch(...)
        {
            entry = co_await this->lookup(identifier);
            this->_cache[identifier] = entry;
        }

        :: drop :: connection sync_connection = co_await this->_connector.connect(entry.address);

        pool :: connection connection = this->_pool.bind(sync_connection);

        co_await connection.send(this->_signer.publickey());
        co_await connection.send(this->_keyexchanger.publickey());
        co_await connection.send(this->_signer.sign(signatures :: entry, this->_keyexchanger.publickey()));

        co_await connection.authenticate(this->_keyexchanger, entry.publickey);

        co_return (class connection){sync_connection, identifier};
    }

    // Private methods

    promise <directory :: client :: entry> directory :: client :: lookup(signature :: publickey identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_connector.connect(this->_server));

            co_await connection.send(false);
            co_await connection.send(identifier);

            if(!(co_await connection.receive <bool> ()))
                throw exceptions :: lookup_failed();

            entry entry;

            entry.address = co_await connection.receive <address> ();
            entry.publickey = co_await connection.receive <class keyexchanger :: publickey> ();

            signature signature = co_await connection.receive <class signature> ();

            verifier verifier(identifier);
            verifier.verify(signature, signatures :: entry, entry.publickey);

            entry.time = now;
            co_return entry;
        }
        catch(...)
        {
            throw exceptions :: lookup_failed();
        }
    }

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
