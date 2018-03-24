// Includes

#include "brahms.hpp"

namespace poseidon
{
    using namespace drop;

    // node

    // Private constructors

    brahms :: node :: node(const signature :: publickey & identifier, brahms & brahms) : _identifier(identifier), _brahms(brahms)
    {
    }

    // Methods

    promise <directory :: connection> brahms :: node :: connect() const
    {
        return this->_brahms._directory.connect(this->_identifier);
    }

    // brahms

    // Constructors

    brahms :: brahms(const signature :: publickey (& view)[settings :: view :: size], const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

        this->run();
    }

    brahms :: brahms(const class signer & signer, const signature :: publickey (& view)[settings :: view :: size], const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _signer(signer), _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            this->_view[i] = view[i];
            this->dispatch(view[i]);
        }

        this->run();
    }

    // Getters

    const signature :: publickey & brahms :: identifier() const
    {
        return this->_signer.publickey();
    }

    signer & brahms :: signer()
    {
        return this->_signer;
    }

    // Private methods

    void brahms :: dispatch(const signature :: publickey & identifier)
    {
        for(size_t i = 0; i < settings :: sample :: size; i++)
            this->_sample[i].next(identifier);
    }

    promise <void> brahms :: pull(signature :: publickey identifier, size_t slot)
    {
        try
        {
            std :: cout << "Connecting" << std :: endl;
            pool :: connection connection = this->_pool.bind(co_await this->_directory.connect(identifier));
            if(log) std :: cout << "Connected" << std :: endl;
        }
        catch(...)
        {

        }
    }

    promise <void> brahms :: run()
    {
        while(true)
        {
            try
            {
                co_await this->_crontab.wait(settings :: interval);

                signature :: publickey requests[settings :: alpha];
                for(size_t i = 0; i < settings :: alpha; i++)
                    requests[i] = this->_view[randombytes_uniform(settings :: alpha)];

                this->_mutex.lock();

                this->_version++;
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    this->_pullslots[i].completed = false;
                    this->pull(requests[i], i);
                }

                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
