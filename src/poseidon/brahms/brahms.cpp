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

    brahms :: brahms(const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _view(view), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
            this->update_sample(view[i]);

        this->_dialer.on <dial> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial));
        });

        this->run();
    }

    brahms :: brahms(const class signer & signer, const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _view(view), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
            this->update_sample(view[i]);

        this->_dialer.on <dial> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial));
        });

        this->run();
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

    // Private methods

    void brahms :: update_sample(const vine :: identifier & identifier)
    {
        for(size_t i = 0; i < settings :: sample :: size; i++)
            this->_sample[i].next(identifier);
    }

    promise <void> brahms :: pull(vine :: identifier identifier, size_t slot, size_t version)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));
            co_await connection.send(false);

            std :: array <vine :: identifier, settings :: view :: size> view;
            for(size_t i = 0; i < settings :: view :: size; i++)
                view[i] = co_await connection.receive <vine :: identifier> ();

            this->_mutex.lock();
            if(this->_version == version)
            {
                this->_pullslots[slot].view = view;
                this->_pullslots[slot].completed = true;
            }
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
            co_await connection.send(this->_signer.publickey());
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: serve(pool :: connection connection)
    {
        try
        {
            bool push = co_await connection.receive <bool> ();

            if(push)
            {
                vine :: identifier identifier = co_await connection.receive <vine :: identifier> ();
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

                this->_version++;

                log << "Sending pull requests" << std :: endl;

                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    this->_pullslots[i].completed = false;
                    this->pull(this->_view[randombytes_uniform(settings :: alpha)], i, this->_version);
                }

                log << "Sending push requests" << std :: endl;

                for(size_t i = 0; i < settings :: beta; i++)
                    this->push(this->_view[randombytes_uniform(settings :: beta)]);

                this->_mutex.unlock();

                co_await this->_crontab.wait(settings :: interval);

                log << "Time's up" << std :: endl;

                for(size_t i = 0; i < settings :: alpha; i++)
                    log << "Slot " << i << (this->_pullslots[i].completed ? " was completed": " was not completed") << std :: endl;
            }
            catch(...)
            {
            }
        }
    }
};
