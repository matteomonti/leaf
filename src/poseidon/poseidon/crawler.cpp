// Includes

#include "crawler.h"
#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    crawler :: crawler(const std :: array <vine :: identifier, brahms :: settings :: view :: size> & view, settings :: server & server, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _brahms(this->_signer, view, dialer, pool, crontab), _server(server), _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }

    crawler :: crawler(const class signer & signer, const std :: array <vine :: identifier, brahms :: settings :: view :: size> & view, settings :: server & server, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _brahms(this->_signer, view, dialer, pool, crontab), _server(server), _dialer(dialer), _pool(pool), _crontab(crontab)
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

    // Methods

    void crawler :: start()
    {
        auto handler = [=](const vine :: identifier & identifier)
        {
            this->_mutex.lock();
            bool block = (this->score(identifier) < settings :: thresholds :: push);
            this->_mutex.unlock();

            return block;
        };

        this->_brahms.on <events :: push :: receive> (handler);
        this->_brahms.on <events :: push :: send> (handler);

        this->_dialer.on <settings :: channel> ([=](const dial & dial)
        {
            try
            {
                this->serve(dial.identifier(), this->_pool.bind(dial));
            }
            catch(...)
            {
            }
        });

        this->_brahms.on <events :: view :: join> ([=](const vine :: identifier & identifier)
        {
            this->maintain(identifier);
        });

        this->_brahms.on <events :: sample :: join> ([=](const vine :: identifier & identifier)
        {
            this->maintain(identifier);
        });

        this->_brahms.on <events :: view :: leave> ([=](const vine :: identifier & identifier)
        {
            this->drop(identifier);
        });

        this->_brahms.on <events :: sample :: leave> ([=](const vine :: identifier & identifier)
        {
            this->drop(identifier);
        });
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

    promise <void> crawler :: serve(vine :: identifier identifier, pool :: connection connection)
    {
        this->_mutex.lock();
        bool duplicate = this->_connections.count(identifier);
        if(!duplicate) this->_connections.insert(identifier);
        this->_mutex.unlock();

        if(duplicate)
            throw "Already serving"; // TODO: Make appropriate exception

        try
        {
            co_await this->_server.serve(connection);

            this->_mutex.lock();
            this->increment(identifier);
            this->_connections.erase(identifier);
            this->_mutex.unlock();
        }
        catch(...)
        {
            this->_mutex.lock();
            this->_connections.erase(identifier);
            this->_mutex.unlock();

            std :: rethrow_exception(std :: current_exception());
        }
    }

    promise <void> crawler :: maintain(vine :: identifier identifier)
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

            try
            {
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (identifier));
                this->serve(identifier, connection);
            }
            catch(...)
            {
                co_await this->_crontab.wait(interval :: random(settings :: intervals :: retry));
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
