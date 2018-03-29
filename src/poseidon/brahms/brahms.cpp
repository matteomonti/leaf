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
        // return this->_brahms._dialer.connect(*this);
        return promise <dial> :: resolved(this->_brahms._dialer.connect(*this));
    }

    // brahms

    // Constructors

    brahms :: brahms(const vine :: identifier (& view)[settings :: view :: size], /*const address & directory*/ dialers :: local :: server & server, connectors :: tcp :: async & connector, pool & pool, crontab & crontab, std :: ostream & log) : _dialer(server, this->_signer /*, connector, pool, crontab*/), _connector(connector), _pool(pool), _crontab(crontab), log(log)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

        this->_dialer.on <dial> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial));
        });

        this->run();
    }

    brahms :: brahms(const class signer & signer, const vine :: identifier (& view)[settings :: view :: size], /*const address & directory*/ dialers :: local :: server & server, connectors :: tcp :: async & connector, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _dialer(server, this->_signer /*, connector, pool, crontab*/), _connector(connector), _pool(pool), _crontab(crontab), log(log)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

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

    void brahms :: dispatch(const vine :: identifier & identifier)
    {
        for(size_t i = 0; i < settings :: sample :: size; i++)
            this->_sample[i].next(identifier);
    }

    promise <void> brahms :: pull(size_t version, vine :: identifier identifier, size_t slot)
    {
        try
        {
            // pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));
            pool :: connection connection = this->_pool.bind(this->_dialer.connect(identifier));
            co_await connection.send(false);

            vine :: identifier view[settings :: view :: size];
            for(size_t i = 0; i < settings :: view :: size; i++)
                view[i] = co_await connection.receive <vine :: identifier> ();

            this->_mutex.lock();
            if(this->_version == version)
            {
                std :: copy(std :: begin(view), std :: end(view), this->_pullslots[slot].view);
                this->_pullslots[slot].completed = true;
            }
            this->_mutex.unlock();
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
            }
            else
            {
                vine :: identifier view[settings :: view :: size];
                this->snapshot(view);

                for(size_t i = 0; i < settings :: view :: size; i++)
                    co_await connection.send <vine :: identifier> (view[i]);
            }
        }
        catch(...)
        {
        }
    }

    void brahms :: snapshot(vine :: identifier (& view)[settings :: view :: size])
    {
        this->_mutex.lock();
        std :: copy(std :: begin(this->_view), std :: end(this->_view), view);
        this->_mutex.unlock();
    }

    promise <void> brahms :: run()
    {
        co_await this->_crontab.wait(settings :: interval);

        while(true)
        {
            try
            {
                log << "Sending pull requests" << std :: endl;

                this->_mutex.lock();

                this->_version++;
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    this->_pullslots[i].completed = false;
                    this->pull(this->_version, this->_view[randombytes_uniform(settings :: alpha)], i);
                }

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
