// Includes

#include "checker.h"

namespace poseidon
{
    using namespace drop;

    // server

    // Constructors

    checker :: server :: server(const pool :: connection & connection, std :: unordered_map <index, vote, shorthash> & votes, std :: recursive_mutex & mutex) : _connection(connection), _votes(votes), _sending(true), _receiving(true), _mutex(mutex)
    {
        this->send();
        this->receive();
    }

    // Getters

    bool checker :: server :: alive()
    {
        this->_mutex.lock();
        bool alive = (this->_sending || this->_receiving);
        this->_mutex.unlock();

        return alive;
    }

    // Methods

    void checker :: server :: push(const statement & statement)
    {
        this->_mutex.lock();

        if(this->_indexes.count(statement.index()))
            this->_pipe.push(statement);

        this->_mutex.unlock();
    }

    // Private methods

    promise <void> checker :: server :: send()
    {
        try
        {
            while(true)
            {
                statement statement = co_await this->_pipe.pop();
                this->_connection.send(statement);
            }
        }
        catch(...)
        {
        }

        this->_mutex.lock();
        this->_sending = false;
        this->_mutex.unlock();
    }

    promise <void> checker :: server :: receive()
    {
        try
        {
            while(true)
            {
                try
                {
                    index index = co_await this->_connection.receive <class index> ();

                    this->_mutex.lock();

                    try
                    {
                        const vote & vote = this->_votes.at(index);
                        this->_pipe.push(statement(index, vote.value));
                    }
                    catch(...)
                    {
                        this->_indexes.insert(index);
                    }

                    this->_mutex.unlock();
                }
                catch(const sockets :: exceptions :: receive_timeout &)
                {
                }
            }
        }
        catch(...)
        {
        }

        this->_mutex.lock();
        this->_receiving = false;
        this->_mutex.unlock();
    }
};
