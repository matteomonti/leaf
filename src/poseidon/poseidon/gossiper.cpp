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

    // Constructors

    gossiper :: gossiper(const identifier & identifier, typename settings :: handler & handler, crontab & crontab) : _identifier(identifier), _locks(0), _handler(handler), _crontab(crontab)
    {
        this->run();
    }

    // Methods

    void gossiper :: add(const statement & statement)
    {
        this->_mutex.lock();
        this->_addbuffer.insert(statement);
        this->_mutex.unlock();
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
        this->lock();

        try
        {
            for(size_t round = 0; round < 5; round++)
            {
                if(this->merging())
                    throw exceptions :: merge_in_progress();

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
                            this->_addbuffer.insert(statement);

                        this->_mutex.unlock();
                    }

                    co_await connection.send(round.view);

                    if(round.view.size() == 0)
                        break;
                }

                co_await this->_crontab.wait(5_s); // TODO: Find better strategy: maybe sync on the fly for thirty seconds?
            }
        }
        catch(...)
        {
            this->unlock();
            std :: rethrow_exception(std :: current_exception());
        }

        this->unlock();
    }

    promise <void> gossiper :: run()
    {
        while(true)
        {
            co_await this->wait_merge();

            while(this->locked())
                co_await this->_crontab.wait(settings :: intervals :: retry);

            for(const statement & statement : this->_addbuffer)
            {
                this->_handler.gossip(statement);
                this->_statements.add(statement);
            }

            this->_addbuffer.clear();
        }
    }
};
