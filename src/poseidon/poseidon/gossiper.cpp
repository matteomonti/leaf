// Includes

#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    gossiper :: gossiper(const signer & signer, brahms & brahms, dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _brahms(brahms), _gossiper(crontab), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    // Methods

    void gossiper :: start()
    {
        this->_dialer.on <1> ([=](const dial & dial)
        {
            this->log << "Connection incoming from " << dial.identifier() << std :: endl;
            this->serve(this->_pool.bind(dial), dial.identifier());
        });

        for(size_t index = 0; index < settings :: sample :: size; index++)
            this->maintain(index);

        // this->ban();
    }

    // Private methods

    promise <void> gossiper :: serve(pool :: connection connection, identifier identifier)
    {
        log << "Serving connection with " << identifier << std :: endl;

        timestamp begin = now;

        promise <void> promise = this->_gossiper.serve(connection, (this->_signer.publickey() < identifier)).until(begin + settings :: gossiper :: intervals :: gossip);
        co_await promise;

        timestamp end = now;

        log << "Connection with " << identifier << " completed, it lasted for " << (end - begin) << std :: endl;

        if(end - begin > settings :: gossiper :: intervals :: reward)
        {
            this->_mutex.lock();

            log << "A reward should be awarded!" << std :: endl;

            try
            {
                int32_t score = this->_scores.at(identifier);
                this->_scores[identifier] = score + 1;
            }
            catch(...)
            {
                this->_scores[identifier] = 1;
            }

            log << "The new score of " << identifier << " is " << this->_scores[identifier] << std :: endl;

            this->_mutex.unlock();
        }
    }

    promise <void> gossiper :: maintain(size_t index)
    {
        while(true)
        {
            try
            {
                identifier identifier = this->_brahms[index];

                log << "Maintaining " << identifier << std :: endl;
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <1> (identifier));
                co_await this->serve(connection, identifier);
            }
            catch(const std :: exception & exception)
            {
                log << "Exception: " << exception.what() << std :: endl;
            }
            catch(...)
            {
                log << "Unknown exception" << std :: endl;
            }

            co_await this->_crontab.wait(5_s);
        }
    }

    promise <void> gossiper :: ban()
    {
        while(true)
        {
            co_await this->_crontab.wait(settings :: gossiper :: intervals :: ban);

            this->_mutex.lock();

            for(auto iterator = this->_scores.begin(); iterator != this->_scores.end();)
            {
                iterator->second--;
                if(iterator->second <= settings :: gossiper :: thresholds :: ban)
                {
                    this->_brahms.ban(iterator->first);
                    iterator = this->_scores.erase(iterator);
                }
                else
                    iterator++;
            }

            for(size_t index = 0; index < settings :: sample :: size; index++)
            {
                identifier identifier = this->_brahms[index];

                try
                {
                    this->_scores.at(identifier);
                }
                catch(...)
                {
                    this->_scores[identifier] = 0;
                }
            }

            this->_mutex.unlock();
        }
    }
};
