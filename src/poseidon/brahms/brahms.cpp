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

    brahms :: brahms(const vine :: identifier (& view)[settings :: view :: size], /*const address & directory*/ dialers :: local :: server & server, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _dialer(server, this->_signer /*, connector, pool, crontab*/), _connector(connector), _pool(pool), _crontab(crontab)
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

    brahms :: brahms(const class signer & signer, const vine :: identifier (& view)[settings :: view :: size], /*const address & directory*/ dialers :: local :: server & server, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _signer(signer), _dialer(server, this->_signer /*, connector, pool, crontab*/), _connector(connector), _pool(pool), _crontab(crontab)
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

    promise <void> brahms :: pull(vine :: identifier identifier, size_t slot)
    {
        try
        {
            // pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));
            pool :: connection connection = this->_pool.bind(this->_dialer.connect(identifier));
        }
        catch(...)
        {
        }

        return promise <void> :: resolved();
    }

    promise <void> brahms :: serve(pool :: connection connection)
    {
        std :: cout << "Connection incoming" << std :: endl;
        return promise <void> :: resolved();
    }

    promise <void> brahms :: run()
    {
        while(true)
        {
            co_await this->_crontab.wait(settings :: interval);

            try
            {
                size_t pullidx[settings :: alpha];
                for(size_t i = 0; i < settings :: alpha; i++)
                    pullidx[i] = randombytes_uniform(settings :: alpha);

                this->_mutex.lock();

                this->_version++;
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    this->_pullslots[i].completed = false;
                    this->pull(this->_view[pullidx[i]], i);
                }

                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
