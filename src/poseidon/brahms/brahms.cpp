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
        return this->_brahms._directory.connect(*this);
    }

    // brahms

    // Constructors

    brahms :: brahms(const vine :: identifier (& view)[settings :: view :: size], const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

        this->_directory.on <dial> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial));
        });

        this->run();
    }

    brahms :: brahms(const class signer & signer, const vine :: identifier (& view)[settings :: view :: size], const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _signer(signer), _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

        this->_directory.on <dial> ([=](const dial & dial)
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
            pool :: connection connection = this->_pool.bind(co_await this->_directory.connect(identifier));
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: serve(pool :: connection connection)
    {
        return promise <void> :: resolved();
    }

    promise <void> brahms :: run()
    {
        while(true)
        {
            std :: cout << "Sleeping" << std :: endl;
            co_await this->_crontab.wait(settings :: interval);
            std :: cout << "Waking" << std :: endl;

            try
            {
                std :: cout << "Sampling requests" << std :: endl;
                vine :: identifier requests[settings :: alpha];
                for(size_t i = 0; i < settings :: alpha; i++)
                    requests[i] = this->_view[randombytes_uniform(settings :: alpha)];

                std :: cout << "Locking" << std :: endl;
                this->_mutex.lock();

                std :: cout << "Pulling" << std :: endl;
                this->_version++;
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    std :: cout << "Slot " << i << std :: endl;
                    this->_pullslots[i].completed = false;
                    this->pull(requests[i], i);
                    std :: cout << "Slot " << i << " completed" << std :: endl;
                }

                std :: cout << "Unlocking" << std :: endl;
                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
