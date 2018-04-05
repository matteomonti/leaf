// Includes

#include "brahms.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // node

    // Private constructors

    brahms :: node :: node(const vine :: identifier & identifier, brahms & brahms) : vine :: identifier(identifier), _brahms(brahms)
    {
    }

    // Methods

    promise <dial> brahms :: node :: connect() const
    {
        return this->_brahms._dialer.connect(*this);
    }

    // brahms

    // Constructors

    brahms :: brahms(const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _view(view), _pullpool(this->_signer.publickey()), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    brahms :: brahms(const class signer & signer, const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _view(view), _pullpool(this->_signer.publickey()), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    // Getters

    const identifier & brahms :: identifier() const
    {
        return this->_signer.publickey();
    }

    signer & brahms :: signer()
    {
        return this->_signer;
    }

    // Methods

    void brahms :: start()
    {
        this->_view.distinct([&](const vine :: identifier & identifier)
        {
            this->_sample.update(identifier);
            this->emit <events :: view :: join> (identifier);
        });

        this->_dialer.on <dial> ([=](const dial & dial)
        {
            this->serve(dial.identifier(), this->_pool.bind(dial));
        });

        this->run();
    }

    // Private methods

    promise <void> brahms :: pull(size_t version, size_t slot, vine :: identifier identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));

            co_await connection.send(false);

            std :: array <vine :: identifier, settings :: view :: size> view;
            for(size_t i = 0; i < settings :: view :: size; i++)
                view[i] = co_await connection.receive <vine :: identifier> ();

            this->_mutex.lock();
            this->_pullpool.set(version, slot, view);
            this->_mutex.unlock();
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: push(vine :: identifier identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));
            co_await connection.send(true);
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: serve(vine :: identifier identifier, pool :: connection connection)
    {
        try
        {
            bool push = co_await connection.receive <bool> ();

            if(push)
            {
                if(this->emit <events :: push :: receive> (identifier))
                {
                    this->_mutex.lock();
                    this->_pushpool.push(identifier);
                    this->_mutex.unlock();
                }
            }
            else
            {
                this->_mutex.lock();
                std :: array <vine :: identifier, settings :: view :: size> view = this->_view;
                this->_mutex.unlock();

                for(size_t i = 0; i < settings :: view :: size; i++)
                    co_await connection.send <vine :: identifier> (view[i]);
            }
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: run()
    {
        co_await this->_crontab.wait(settings :: interval);

        while(true)
        {
            try
            {
                this->_mutex.lock();

                log << "Initializing pushpool" << std :: endl;

                this->_pushpool.init();
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    vine :: identifier identifier = this->_view.random();
                    log << "Sending push request to " << identifier << std :: endl;

                    if(this->emit <events :: push :: send> (identifier))
                        this->push(identifier);
                }

                log << std :: endl << "Initializing pullpool" << std :: endl;
                size_t version = this->_pullpool.init();
                for(size_t i = 0; i < settings :: beta; i++)
                {
                    vine :: identifier identifier = this->_view.random();
                    log << "Sending pull request to " << identifier << std :: endl;

                    this->pull(version, i, identifier);
                }

                this->_mutex.unlock();

                co_await this->_crontab.wait(settings :: interval);

                this->_mutex.lock();

                log << std :: endl << "Pushpool " << (this->_pushpool ? "ok" : "not ok") << std :: endl;
                log << "Pullpool " << (this->_pullpool ? "ok" : "not ok") << std :: endl;
                log << "Sample " << (this->_sample ? "ok" : "not ok") << std :: endl;

                if(this->_pushpool && this->_pullpool && this->_sample)
                {
                    log << std :: endl << "All tests passed." << std :: endl;

                    view <settings :: view :: size> next = view <settings :: view :: size> :: from <settings :: alpha, settings :: beta, settings :: gamma> (this->_pushpool, this->_pullpool, this->_sample);

                    for(size_t i = 0; i < settings :: view :: size; i++)
                        log << i << ": " << this->_view[i] << std :: endl;

                    log << "Updating sample" << std :: endl;

                    next.distinct([&](const vine :: identifier & identifier)
                    {
                        this->_sample.update(identifier);
                    });

                    log << std :: endl << "View comparison (old - new):" << std :: endl;

                    for(size_t i = 0; i < settings :: view :: size; i++)
                        log << i << ": " << this->_view[i] << "\t" << next[i] << std :: endl;

                    log << std :: endl << "Emitting differences:" << std :: endl;

                    this->_view.diff(next, [&](const vine :: identifier & join)
                    {
                        this->emit <events :: view :: join> (join);
                    }, [&](const vine :: identifier & leave)
                    {
                        this->emit <events :: view :: leave> (leave);
                    });

                    log << std :: endl << "Setting new view" << std :: endl;

                    this->_view = next;

                    log << std :: endl << std :: endl << "------------------------------------------------" << std :: endl << std :: endl << std :: endl;
                }

                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
