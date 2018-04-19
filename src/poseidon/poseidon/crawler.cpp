// Includes

#include "crawler.h"
#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Exceptions

    const char * crawler :: exceptions :: already_serving :: what() const throw()
    {
        return "Already serving.";
    }

    // Constructors

    crawler :: crawler(const std :: array <vine :: identifier, brahms :: settings :: view :: size> & view, settings :: server & server, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _brahms(this->_signer, view, dialer, pool, crontab), _server(server), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    crawler :: crawler(const class signer & signer, const std :: array <vine :: identifier, brahms :: settings :: view :: size> & view, settings :: server & server, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _brahms(this->_signer, view, dialer, pool, crontab), _server(server), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    // Getters

    const identifier & crawler :: identifier() const
    {
        return this->_signer.publickey();
    }

    signer & crawler :: signer()
    {
        return this->_signer;
    }

    std :: array <optional <vine :: identifier>, brahms :: settings :: sample :: size> crawler :: sample()
    {
        return this->_brahms.sample();
    }

    // Methods

    void crawler :: start()
    {
        auto handler = [=](const vine :: identifier & identifier)
        {
            this->_mutex.lock();
            bool pass = (this->score(identifier) >= settings :: thresholds :: push);
            this->_mutex.unlock();

            return pass;
        };

        this->_brahms.on <events :: push :: receive> (handler);
        this->_brahms.on <events :: push :: send> (handler);

        this->_dialer.on <settings :: channel> ([=](const dial & dial) -> promise <void>
        {
            try
            {
                co_await this->serve(dial.identifier(), this->_pool.bind(dial));
            }
            catch(...)
            {
            }
        });

        this->_brahms.on <events :: view :: join> ([=](const vine :: identifier & identifier)
        {
            this->maintain(identifier, true);
        });

        this->_brahms.on <events :: sample :: join> ([=](const vine :: identifier & identifier)
        {
            this->maintain(identifier, false);
        });

        this->_brahms.on <events :: view :: leave> ([=](const vine :: identifier & identifier)
        {
            this->drop(identifier);
        });

        this->_brahms.on <events :: sample :: leave> ([=](const vine :: identifier & identifier)
        {
            this->drop(identifier);
        });

        this->_brahms.start();
    }

    // Private methods

    uint64_t crawler :: score(const vine :: identifier & identifier)
    {
        try
        {
            return this->_scores.at(identifier);
        }
        catch(...)
        {
            return 0;
        }
    }

    void crawler :: increment(const vine :: identifier & identifier)
    {
        try
        {
            uint64_t current = this->_scores.at(identifier);
            this->_scores[identifier] = current + 1;
        }
        catch(...)
        {
            this->_scores[identifier] = 1;
        }

        this->_crontab.wait(settings :: intervals :: window).then([=]()
        {
            this->_mutex.lock();
            this->decrement(identifier);
            this->_mutex.unlock();
        });
    }

    void crawler :: decrement(const vine :: identifier & identifier)
    {
        uint64_t current = this->_scores[identifier];

        if(current > 1)
            this->_scores[identifier] = current - 1;
        else
            this->_scores.erase(identifier);
    }

    promise <void> crawler :: serve(vine :: identifier identifier, bool push)
    {
        log << "[crawler] Establishing connection to " << identifier << std :: endl;

        this->_mutex.lock();
        bool duplicate = this->_connections.count(identifier);
        if(!duplicate) this->_connections.insert(identifier);
        this->_mutex.unlock();

        if(duplicate)
        {
            log << "[crawler] It would be a duplicate, forget about it" << std :: endl;
            // throw exceptions :: already_serving();
        }

        try
        {
            log << "[crawler] Connecting" << std :: endl;
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (identifier));
            log << "[crawler] Connected" << std :: endl;

            co_await connection.send(push);

            log << "[crawler] (Established) serving" << std :: endl;
            co_await this->_server.serve(identifier, connection, true);
            log << "[crawler] (Established) served successfully" << std :: endl;

            this->_mutex.lock();
            this->increment(identifier);
            this->_connections.erase(identifier);
            this->_mutex.unlock();
        }
        catch(const std :: exception & exception)
        {
            log << "[crawler] (Established serve) there was an exception: " << exception.what() << std :: endl;
            this->_mutex.lock();
            this->_connections.erase(identifier);
            this->_mutex.unlock();

            std :: rethrow_exception(std :: current_exception());
        }
    }

    promise <void> crawler :: serve(vine :: identifier identifier, pool :: connection connection)
    {
        log << "[crawler] Serving connection from " << identifier << std :: endl;

        this->_mutex.lock();
        bool duplicate = this->_connections.count(identifier);
        if(!duplicate) this->_connections.insert(identifier);
        this->_mutex.unlock();

        if(duplicate)
        {
            log << "[crawler] It's a duplicate, throw it away!" << std :: endl;
            // throw exceptions :: already_serving();
        }

        try
        {
            bool pull = co_await connection.receive <bool> ();

            log << "[crawler] (Incoming) serving" << std :: endl;
            co_await this->_server.serve(identifier, connection, pull);
            log << "[crawler] (Incoming) served successfully" << std :: endl;

            this->_mutex.lock();
            this->increment(identifier);
            this->_connections.erase(identifier);
            this->_mutex.unlock();
        }
        catch(const std :: exception & exception)
        {
            log << "[crawler] (Incoming) exception: " << exception.what() << std :: endl;
            this->_mutex.lock();
            this->_connections.erase(identifier);
            this->_mutex.unlock();

            std :: rethrow_exception(std :: current_exception());
        }
    }

    promise <void> crawler :: maintain(vine :: identifier identifier, bool push)
    {
        this->_mutex.lock();
        this->_neighborhood.insert(identifier);
        this->_mutex.unlock();

        while(true)
        {
            this->_mutex.lock();
            bool alive = this->_neighborhood.count(identifier);
            this->_mutex.unlock();

            if(!alive)
                break;

            bool exception;

            try
            {
                co_await this->serve(identifier, push);
                exception = false;
            }
            catch(...)
            {
                exception = true;
            }

            if(exception)
            {
                interval retry = interval :: random(settings :: intervals :: retry);
                co_await this->_crontab.wait(retry);
            }
        }
    }

    void crawler :: drop(const vine :: identifier & identifier)
    {
        this->_mutex.lock();
        this->_neighborhood.erase(this->_neighborhood.find(identifier));
        this->_mutex.unlock();
    }
};
