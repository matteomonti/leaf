// Includes

#include "checker.h"
#include "checkpool.hpp"

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

    void checker :: server :: serve(const index & index)
    {
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
            std :: vector <index> indexes = co_await this->_connection.receive <std :: vector <index>> ();

            for(const index & index : indexes)
                this->serve(index);

            while(true)
            {
                try
                {
                    index index = co_await this->_connection.receive <class index> ();
                    this->serve(index);
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

    // client

    // Constructors

    checker :: client :: client(const pool :: connection & connection, const size_t & version, const size_t & slot, const std :: vector <index> & indexes, checkpool <brahms :: settings :: sample :: size> & checkpool, std :: recursive_mutex & mutex) : _connection(connection), _version(version), _slot(slot), _checkpool(checkpool), _mutex(mutex)
    {
        this->send(indexes);
        this->receive();
    }

    // Methods

    promise <void> checker :: client :: close()
    {
        this->_pipe.close();
        this->_alive = false;
        return this->_close;
    }

    // Private methods

    promise <void> checker :: client :: send(const std :: vector <index> & indexes)
    {
        try
        {
            co_await this->_connection.send(indexes);

            while(true)
            {
                index index = co_await this->_pipe.pop();
                this->_connection.send(index);
            }
        }
        catch(...)
        {
        }
    }

    promise <void> checker :: client :: receive()
    {
        try
        {
            while(true)
            {
                try
                {
                    statement statement = co_await this->_connection.receive <class statement> ();

                    this->_mutex.lock();
                    this->_checkpool.set(this->_version, this->_slot, statement.index(), {statement.value(), statement.signature()});
                    this->_mutex.unlock();
                }
                catch(const sockets :: exceptions :: receive_timeout &)
                {
                }

                if(!(this->_alive))
                    break;
            }
        }
        catch(...)
        {
        }

        this->_close.resolve();
    }
};
