// Includes

#include "brahms.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    brahms :: brahms(const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _view(view), _pullpool(this->_signer.publickey()), _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }

    brahms :: brahms(const class signer & signer, const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _view(view), _pullpool(this->_signer.publickey()), _dialer(dialer), _pool(pool), _crontab(crontab)
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
            this->_sample.update(identifier, [&](const vine :: identifier & add)
            {
                this->emit <events :: sample :: join> (add);
            }, [&](const vine :: identifier & remove)
            {
                this->emit <events :: sample :: leave> (remove);
            });

            this->emit <events :: view :: join> (identifier);
        });

        this->_dialer.on <settings :: channel> ([=](const dial & dial)
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
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (identifier));

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
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (identifier));
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

                this->_pushpool.init();
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    vine :: identifier identifier = this->_view.random();

                    if(this->emit <events :: push :: send> (identifier))
                        this->push(identifier);
                }

                size_t version = this->_pullpool.init();
                for(size_t i = 0; i < settings :: beta; i++)
                    this->pull(version, i, this->_view.random());

                this->_mutex.unlock();

                co_await this->_crontab.wait(settings :: interval);

                this->_mutex.lock();

                if(this->_pushpool && this->_pullpool && this->_sample)
                {
                    view <settings :: view :: size> next = view <settings :: view :: size> :: from <settings :: alpha, settings :: beta, settings :: gamma> (this->_pushpool, this->_pullpool, this->_sample);

                    next.distinct([&](const vine :: identifier & identifier)
                    {
                        this->_sample.update(identifier, [&](const vine :: identifier & add)
                        {
                            this->emit <events :: sample :: join> (add);
                        }, [&](const vine :: identifier & remove)
                        {
                            this->emit <events :: sample :: leave> (remove);
                        });
                    });

                    this->_view.diff(next, [&](const vine :: identifier & join)
                    {
                        this->emit <events :: view :: join> (join);
                    }, [&](const vine :: identifier & leave)
                    {
                        this->emit <events :: view :: leave> (leave);
                    });

                    this->_view = next;
                }

                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
