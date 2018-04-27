// Includes

#include "coordinator.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    coordinator :: coordinator(const uint16_t & port, const size_t & nodes) : _nodes(nodes), _acceptor(port)
    {
        this->_acceptor.on <connection> ([=](const connection & connection)
        {
            this->serve(this->_pool.bind(connection));
        });
    }

    // Private methods

    promise <void> coordinator :: serve(pool :: connection connection)
    {
        try
        {
            identifier identifier = co_await connection.receive <vine :: identifier> ();
            uint32_t size = co_await connection.receive <uint32_t> ();

            this->_mutex.lock();

            if(!(this->_identifiers.vector.size()))
            {
                this->_identifiers.set.insert(identifier);
                std :: cout << "There are " << this->_identifiers.set.size() << " nodes connected." << std :: endl;
                if(this->_identifiers.set.size() == this->_nodes)
                    for(const vine :: identifier & identifier : this->_identifiers.set)
                        this->_identifiers.vector.push_back(identifier);
            }

            this->_mutex.unlock();

            while(true)
            {
                bool keepalive = co_await connection.receive <bool> ();

                bool ready = false;
                std :: vector <vine :: identifier> view;

                this->_mutex.lock();

                if(this->_identifiers.vector.size())
                {
                    ready = true;
                    for(size_t i = 0; i < size; i++)
                        while(true)
                        {
                            vine :: identifier random = this->_identifiers.vector[randombytes_uniform(this->_identifiers.vector.size())];
                            if(random == identifier)
                                continue;

                            view.push_back(random);
                            break;
                        }
                }

                this->_mutex.unlock();

                co_await connection.send(ready);

                if(ready)
                    co_await connection.send(view);
            }
        }
        catch(...)
        {
        }
    }

    // Static methods

    std :: vector <identifier> coordinator :: await(const address & server, const identifier & identifier, const size_t & size)
    {
        while(true)
        {
            try
            {
                connection connection = connectors :: tcp :: sync :: connect(server);
                connection.send(identifier);
                connection.send <uint32_t> (size);

                while(true)
                {
                    connection.send(true);
                    bool ready = connection.receive <bool> ();

                    if(ready)
                    {
                        std :: vector <vine :: identifier> view = connection.receive <std :: vector <vine :: identifier>> ();
                        return view;
                    }

                    sleep(settings :: intervals :: poll);
                }
            }
            catch(...)
            {
            }
        }
    }
};
