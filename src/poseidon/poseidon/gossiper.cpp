// Includes

#include "gossiper.h"

namespace poseidon
{
    // Exceptions

    const char * gossiper :: exceptions :: merge_in_progress :: what() const throw()
    {
        return "Merge in progress.";
    }

    // Constructors

    gossiper :: gossiper(const identifier & identifier, crontab & crontab, std :: ostream & log) : _identifier(identifier), _crontab(crontab), _locks(0), log(log)
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
        log << "Lock: " << this->_locks << std :: endl;
        this->_mutex.unlock();
    }

    void gossiper :: unlock()
    {
        this->_mutex.lock();
        this->_locks--;
        log << "Unlock: " << this->_locks << std :: endl;
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

    promise <void> gossiper :: serve(identifier identifier, pool :: connection connection)
    {
        log << "Serving a connection" << std :: endl;
        this->lock();

        try
        {
            if(this->merging())
            {
                log << "Should be merging, rejecting connection" << std :: endl;
                throw exceptions :: merge_in_progress();
            }

            if(this->_identifier < identifier)
            {
                log << "Initializing sync" << std :: endl;
                co_await connection.send(this->_statements.sync().view);
            }

            while(true)
            {
                log << "Waiting for remote view" << std :: endl;
                syncset <statement> :: view view = co_await connection.receive <syncset <statement> :: view> ();
                log << "Remote view received, its size is " << view.size() << std :: endl;

                if(view.size() == 0)
                    break;

                log << "Syncing remote view" << std :: endl;

                syncset <statement> :: round round = this->_statements.sync(view);

                this->_mutex.lock();

                for(const statement & statement : round.add)
                {
                    log << "Adding " << statement.identifier() << " / " << statement.sequence() << std :: endl;
                    this->_addbuffer.insert(statement);
                }

                this->_mutex.unlock();

                log << "Local view size is " << round.view.size() << std :: endl;
                co_await connection.send(round.view);

                if(round.view.size() == 0)
                    break;
            }

            log << "Sync successfully completed, waiting 30 seconds" << std :: endl;

            co_await this->_crontab.wait(30_s);
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
            log << "Waiting for next merge" << std :: endl;
            co_await this->wait_merge();

            log << "Acquiring lock" << std :: endl;
            while(this->locked())
                co_await this->_crontab.wait(settings :: intervals :: retry);

            log << "Lock acquired" << std :: endl;

            log << "Merging statements" << std :: endl;

            for(const statement & statement : this->_addbuffer)
            {
                log << statement.identifier() << " / " << statement.sequence() << std :: endl;

                this->emit <class statement> (statement);
                this->_statements.add(statement);
            }

            this->_addbuffer.clear();
        }
    }
};
