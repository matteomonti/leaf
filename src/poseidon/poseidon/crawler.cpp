// Includes

#include "crawler.h"
#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

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

    // Methods

    void crawler :: start()
    {
        auto handler = [=](const vine :: identifier & identifier)
        {
            this->log << "Push receive / send: " << identifier << std :: endl;

            this->_mutex.lock();
            bool pass = (this->score(identifier) >= settings :: thresholds :: push);
            this->log << "Score is " << (this->score(identifier)) << ": " << (pass ? "allowing" : "blocking") << std :: endl;
            this->_mutex.unlock();

            return pass;
        };

        this->_brahms.on <events :: push :: receive> (handler);
        this->_brahms.on <events :: push :: send> (handler);

        this->_dialer.on <settings :: channel> ([=](const dial & dial) -> promise <void>
        {
            try
            {
                this->log << "Connection incoming from " << dial.identifier() << ": serving" << std :: endl;
                co_await this->serve(dial.identifier(), this->_pool.bind(dial));
            }
            catch(...)
            {
            }
        });

        this->_brahms.on <events :: view :: join> ([=](const vine :: identifier & identifier)
        {
            this->log << "View join: " << identifier << " (beginning to maintain)" << std :: endl;
            this->maintain(identifier);
        });

        this->_brahms.on <events :: sample :: join> ([=](const vine :: identifier & identifier)
        {
            this->log << "Sample join: " << identifier << " (beginning to maintain)" << std :: endl;
            this->maintain(identifier);
        });

        this->_brahms.on <events :: view :: leave> ([=](const vine :: identifier & identifier)
        {
            this->log << "View leave: " << identifier << " (dropping)" << std :: endl;
            this->drop(identifier);
        });

        this->_brahms.on <events :: sample :: leave> ([=](const vine :: identifier & identifier)
        {
            this->log << "Sample leave: " << identifier << " (dropping)" << std :: endl;
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

    promise <void> crawler :: serve(vine :: identifier identifier, pool :: connection connection)
    {
        log << "Serving " << identifier << std :: endl;

        this->_mutex.lock();
        bool duplicate = this->_connections.count(identifier);
        if(!duplicate) this->_connections.insert(identifier);

        log << "Connection " << (duplicate ? "is" : "is not") << " a duplicate" << std :: endl;
        this->_mutex.unlock();

        if(duplicate)
            throw "Already serving"; // TODO: Make appropriate exception

        try
        {
            log << "Forwarding to server" << std :: endl;
            co_await this->_server.serve(connection);
            log << "Server returned successfully from " << identifier << std :: endl;

            this->_mutex.lock();
            log << "Incrementing " << identifier << std :: endl;
            this->increment(identifier);
            this->_connections.erase(identifier);
            this->_mutex.unlock();
        }
        catch(const std :: exception & exception)
        {
            log << "Server threw an error on " << identifier << ": " << exception.what() << std :: endl;
            this->_mutex.lock();
            this->_connections.erase(identifier);
            this->_mutex.unlock();

            std :: rethrow_exception(std :: current_exception());
        }
    }

    promise <void> crawler :: maintain(vine :: identifier identifier)
    {
        log << "Maintaining " << identifier << std :: endl;

        this->_mutex.lock();
        this->_neighborhood.insert(identifier);
        this->_mutex.unlock();

        while(true)
        {
            log << "Maintenance round for " << identifier << std :: endl;

            this->_mutex.lock();
            bool alive = this->_neighborhood.count(identifier);
            this->_mutex.unlock();

            if(!alive)
            {
                log << "Node was dropped: end of maintenance" << std :: endl;
                break;
            }

            bool exception;

            try
            {
                log << "Connecting" << std :: endl;
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (identifier));
                log << "Serving connection" << std :: endl;
                co_await this->serve(identifier, connection);
                exception = false;
            }
            catch(...)
            {
                exception = true;
            }

            if(exception)
            {
                interval retry = interval :: random(settings :: intervals :: retry);
                log << "Maintenance round failed for " << identifier << std :: endl;
                log << "Waiting for " << retry << std :: endl;
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
