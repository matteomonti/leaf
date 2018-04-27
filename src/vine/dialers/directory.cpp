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

    const identifier & directory :: client :: identifier() const
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

    promise <dial> directory :: client :: connect(vine :: identifier identifier)
    {
        std :: cout << "[directory / connect] Establishing connection." << std :: endl;

        optional <entry> entry;

        this->_mutex.lock();

        std :: cout << "[directory / connect] Looking up entry in cache." << std :: endl;

        try
        {
            entry = this->_cache.at(identifier);
            std :: cout << "[directory / connect] Entry found in cache." << std :: endl;
        }
        catch(...)
        {
            std :: cout << "[directory / connect] Entry not found." << std :: endl;
        }
        
        this->_mutex.unlock();

        if(!entry)
        {
            std :: cout << "[directory / connect] Looking up entry." << std :: endl;
            entry = co_await this->lookup(identifier);

            std :: cout << "[directory / connect] Saving entry in cache." << std :: endl;
            this->_mutex.lock();
            this->_cache[identifier] = *entry;
            this->_mutex.unlock();
        }

        std :: cout << "[directory / connect] Establishing connection." << std :: endl;
        connection sync_connection = co_await this->_connector.connect(entry->address);

        std :: cout << "[directory / connect] Connection established. Binding." << std :: endl;
        pool :: connection connection = this->_pool.bind(sync_connection);

        std :: cout << "[directory / connect] Sending publickey." << std :: endl;
        co_await connection.send(this->_signer.publickey());

        std :: cout << "[directory / connect] Sending keyexchanger publickey." << std :: endl;
        co_await connection.send(this->_keyexchanger.publickey());

        std :: cout << "[directory / connect] Sending timestamp." << std :: endl;
        timestamp timestamp = now;
        co_await connection.send(timestamp);

        std :: cout << "[directory / connect] Sending signature." << std :: endl;
        co_await connection.send(this->_signer.sign(signatures :: entry, this->_keyexchanger.publickey(), timestamp));

        std :: cout << "[directory / connect] Authenticating." << std :: endl;
        co_await connection.authenticate(this->_keyexchanger, entry->publickey);

        std :: cout << "[directory / connect] Returning dial." << std :: endl;
        co_return dial(identifier, sync_connection);
    }

    // Private methods

    promise <directory :: client :: entry> directory :: client :: lookup(vine :: identifier identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_connector.connect(this->_server));

            co_await connection.send(false);
            co_await connection.send(identifier);

            if(!(co_await connection.receive <bool> ()))
                throw exceptions :: node_not_found();

            entry entry;

            entry.address = co_await connection.receive <address> ();
            entry.publickey = co_await connection.receive <class keyexchanger :: publickey> ();

            entry.timestamp = co_await connection.receive <timestamp> ();
            if(timestamp(now) - entry.timestamp > settings :: timeout)
                throw exceptions :: node_not_found();

            signature signature = co_await connection.receive <class signature> ();

            verifier verifier(identifier);
            verifier.verify(signature, signatures :: entry, entry.publickey, entry.timestamp);

            co_return entry;
        }
        catch(...)
        {
            throw exceptions :: node_not_found();
        }
    }

    promise <void> directory :: client :: refresh()
    {
        while(true)
        {
            try
            {
                pool :: connection connection = this->_pool.bind(co_await this->_connector.connect(this->_server));
                co_await connection.send(true);

                co_await connection.send(this->_signer.publickey());
                co_await connection.send(this->_acceptor.port());
                co_await connection.send(this->_keyexchanger.publickey());

                timestamp timestamp = now;
                co_await connection.send(timestamp);

                co_await connection.send(this->_signer.sign(signatures :: entry, this->_keyexchanger.publickey(), timestamp));
            }
            catch(...)
            {
            }

            co_await this->_crontab.wait(settings :: refresh);
        }
    }
};
