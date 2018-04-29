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
        this->_brahms.on <push> ([=](const identifier & identifier)
        {
            bool accept = false;

            this->_mutex.lock();

            try
            {
                accept = (this->_scores.at(identifier) >= settings :: gossiper :: thresholds :: push);
            }
            catch(...)
            {
            }
            this->_mutex.unlock();

            log << "Received push from " << identifier << ": " << (accept ? "accepting it" : "rejecting it") << std :: endl;

            return accept;
        });

        this->_dialer.on <1> ([=](const dial & dial) -> promise <void>
        {
            try
            {
                pool :: connection connection = this->_pool.bind(dial);
                bool sample = co_await connection.receive <bool> ();
                this->serve(connection, dial.identifier(), sample);
            }
            catch(...)
            {
            }
        });

        this->_gossiper.on <statement> ([=](const auto & id, const statement & statement)
        {
            statement.verify();
            
            this->_mutex.lock();
            bool accept = (this->_blacklist.count(id) == 0);
            this->_mutex.unlock();

            if(accept)
            {
                this->log << "Received statement from someone in whose sample I am not. Emitting it." << std :: endl;
                this->emit <class statement> (statement);
            }
            else
                this->log << "Received statement from someone in whose sample I am. Blocking it." << std :: endl;

            return accept;
        });

        for(size_t index = 0; index < settings :: sample :: size; index++)
            this->maintain(true, index);

        for(size_t index = 0; index < settings :: view :: size; index++)
            this->maintain(false, index);

        this->ban();
    }

    void gossiper :: publish(const statement & statement)
    {
        this->_gossiper.publish(statement);
    }

    // Private methods

    promise <void> gossiper :: serve(pool :: connection connection, identifier identifier, bool ignore)
    {
        timestamp begin = now;

        auto handle = this->_gossiper.serve(connection, (this->_signer.publickey() < identifier)).until(begin + settings :: gossiper :: intervals :: gossip);

        if(ignore)
        {
            this->_mutex.lock();
            this->_blacklist.insert(handle);
            this->_mutex.unlock();
        }

        co_await handle;

        timestamp end = now;

        if(end - begin > settings :: gossiper :: intervals :: reward)
        {
            this->_mutex.lock();

            try
            {
                int32_t score = this->_scores.at(identifier);
                this->_scores[identifier] = score + 1;
            }
            catch(...)
            {
                this->_scores[identifier] = 1;
            }

            this->_mutex.unlock();
        }
    }

    promise <void> gossiper :: maintain(bool sample, size_t index)
    {
        while(true)
        {
            try
            {
                identifier identifier = (sample ? this->_brahms[index] : this->_brahms[index]);

                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <1> (identifier));
                co_await connection.send(sample);

                co_await this->serve(connection, identifier, false);
            }
            catch(...)
            {
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

            log << "Lowering scores for everyone" << std :: endl;

            log << "Initial scores snapshot:" << std :: endl;

            for(const auto & score : this->_scores)
                log << score.first << ": " << score.second << std :: endl;

            log << std :: endl;

            for(auto iterator = this->_scores.begin(); iterator != this->_scores.end();)
            {
                iterator->second--;
                if(iterator->second <= settings :: gossiper :: thresholds :: ban)
                {
                    log << "Banning " << iterator->first << std :: endl;
                    this->_brahms.ban(iterator->first);
                    iterator = this->_scores.erase(iterator);
                }
                else
                    iterator++;
            }

            log << std :: endl << "Final scores snapshot:" << std :: endl;

            for(const auto & score : this->_scores)
                log << score.first << ": " << score.second << std :: endl;

            log << std :: endl << std :: endl;

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
