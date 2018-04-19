// Includes

#include "checker.h"

namespace poseidon
{
    using namespace drop;

    // server

    // Constructors

    checker :: server :: server(const pool :: connection & connection, std :: unordered_map <index, vote, shorthash> & votes, std :: mutex & mutex) : _connection(connection), _votes(votes), _mutex(mutex)
    {
        this->send();
        this->receive();
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
    }

    promise <void> checker :: server :: receive()
    {
        try
        {
            while(true)
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
        }
        catch(...)
        {
        }
    }
};
