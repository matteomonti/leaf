// Includes

#include "gossiper.h"
#include "poseidon.h"

namespace poseidon
{
    // Exceptions

    const char * gossiper :: exceptions :: merge_in_progress :: what() const throw()
    {
        return "Merge in progress.";
    }

    // syncer

    // Constructors

    gossiper :: syncer :: syncer(gossiper & gossiper, const pool :: connection & connection, const std :: vector <statement> & addbuffer, std :: ostream & log) : _gossiper(gossiper), _connection(connection), _addbuffer(addbuffer), _alive(true), log(log)
    {
        this->send();
        this->receive();
    }

    // Methods

    void gossiper :: syncer :: push(const statement & statement)
    {
        this->_pipe.push(statement);
    }

    promise <void> gossiper :: syncer :: close()
    {
        this->_alive = false;
        return this->_close;
    }

    // Private methods

    promise <void> gossiper :: syncer :: send()
    {
        try
        {
            log << "[syncer] Sending addbuffer" << std :: endl;
            for(size_t i = 0; i < this->_addbuffer.size(); i++)
                co_await this->_connection.send(this->_addbuffer[i]);

            while(true)
            {
                statement statement = co_await this->_pipe.pop();
                log << "[syncer] Statement pushed through pipe!" << std :: endl;
                co_await this->_connection.send(statement);
                log << "[syncer] Statement sent" << std :: endl;
            }
        }
        catch(const pipe <statement> :: exceptions :: pipe_closing &)
        {
            log << "[gossiper] Pipe closing" << std :: endl;
        }
        catch(...)
        {
            log << "[gossiper] Exception!" << std :: endl;
        }
    }

    promise <void> gossiper :: syncer :: receive()
    {
        try
        {
            while(true)
            {
                try
                {
                    statement statement = co_await this->_connection.receive <class statement> ();

                    log << "[syncer] Received statement! " << statement.identifier() << " / " << statement.sequence() << ": " << statement.value();

                    this->_gossiper._mutex.lock();
                    log << "[syncer] Adding to gossiper" << std :: endl;
                    this->_gossiper.add(statement);
                    this->_gossiper._mutex.unlock();
                }
                catch(const sockets :: exceptions :: receive_timeout &)
                {
                }

                if(!(this->_alive))
                {
                    this->_close.resolve();
                    co_return;
                }
            }
        }
        catch(...)
        {
        }
    }

    // gossiper

    // Constructors

    gossiper :: gossiper(const identifier & identifier, typename settings :: handler & handler, crontab & crontab, std :: ostream & log) : _identifier(identifier), _locks(0), _handler(handler), _crontab(crontab), log(log)
    {
        this->run();
    }

    // Methods

    void gossiper :: add(const statement & statement)
    {
        log << "[gossiper] Adding statement" << std :: endl;

        if(!(this->_addbuffer.count(statement)) && !(this->_statements.find(statement)))
        {
            log << "[gossiper] It's new: inserting into addbuffer" << std :: endl;
            this->_addbuffer.insert(statement);

            log << "[gossiper] Sending to handler" << std :: endl;
            this->_handler.gossip(statement);

            log << "[gossiper] Pushing to syncers" << std :: endl;
            for(const auto & pair : this->_syncers)
                pair.second->push(statement);
        }
    }

    // Private methods

    void gossiper :: lock()
    {
        this->_mutex.lock();
        this->_locks++;
        this->_mutex.unlock();
    }

    void gossiper :: unlock()
    {
        this->_mutex.lock();
        this->_locks--;
        this->_mutex.unlock();
    }

    promise <void> gossiper :: wait_merge()
    {
        this->_mutex.lock();
        this->_nextmerge = timestamp(now) + settings :: intervals :: merge;
        this->_mutex.unlock();

        return this->_crontab.wait(this->_nextmerge);
    }

    bool gossiper :: locked()
    {
        this->_mutex.lock();
        bool locked = this->_locks;
        this->_mutex.unlock();

        return locked;
    }

    bool gossiper :: merging()
    {
        this->_mutex.lock();
        bool merging = timestamp(now) > this->_nextmerge;
        this->_mutex.unlock();

        return merging;
    }

    promise <void> gossiper :: serve(identifier identifier, pool :: connection connection, bool pull)
    {
        log << "[gossiper] Serving connection from " << identifier << std :: endl;

        this->lock();

        try
        {
            if(this->merging())
            {
                log << "[gossiper] Merge in progress" << std :: endl;
                throw exceptions :: merge_in_progress();
            }

            log << "[gossiper] Starting sync with " << identifier << std :: endl;
            if(this->_identifier < identifier)
                co_await connection.send(this->_statements.sync().view);

            while(true)
            {
                syncset <statement> :: view view = co_await connection.receive <syncset <statement> :: view> ();

                if(view.size() == 0)
                    break;

                syncset <statement> :: round round = this->_statements.sync(view);

                if(pull)
                {
                    this->_mutex.lock();

                    for(const statement & statement : round.add)
                        this->add(statement);

                    this->_mutex.unlock();
                }

                co_await connection.send(round.view);

                if(round.view.size() == 0)
                    break;
            }
        }
        catch(...)
        {
            this->unlock();
            std :: rethrow_exception(std :: current_exception());
        }

        this->unlock();

        log << "[gossiper] Sync completed with " << identifier << std :: endl;

        co_await this->_crontab.wait(settings :: intervals :: sync); // REMOVE ME
        co_return; // REMOVE ME

        log << "[gossiper] Copying addbuffer" << std :: endl;
        std :: vector <statement> addbuffer;

        this->_mutex.lock();

        addbuffer.reserve(this->_addbuffer.size());
        for(const statement & statement : this->_addbuffer)
            addbuffer.push_back(statement);

        log << "[gossiper] Creating syncer" << std :: endl;
        syncer syncer(*this, connection, addbuffer, log);
        this->_syncers[identifier] = &syncer;

        this->_mutex.unlock();

        log << "[gossiper] Waiting for sync interval" << std :: endl;
        co_await this->_crontab.wait(settings :: intervals :: sync);

        this->_mutex.lock();
        log << "[gossiper] Removing syncer from syncers pool" << std :: endl;
        this->_syncers.erase(identifier);
        this->_mutex.unlock();

        log << "[gossiper] Closing syncer" << std :: endl;
        co_await syncer.close();

        log << "[gossiper] Syncer closed" << std :: endl;
    }

    promise <void> gossiper :: run()
    {
        while(true)
        {
            co_await this->wait_merge();

            while(this->locked())
                co_await this->_crontab.wait(settings :: intervals :: retry);

            this->_mutex.lock();

            for(const statement & statement : this->_addbuffer)
                this->_statements.add(statement);

            this->_addbuffer.clear();

            this->_mutex.unlock();
        }
    }
};
