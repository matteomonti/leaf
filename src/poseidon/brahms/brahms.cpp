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

    brahms :: brahms(const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
    }

    brahms :: brahms(const class signer & signer, const address & directory, connectors :: tcp :: async & connector, pool & pool, crontab & crontab) : _signer(signer), _directory(directory, this->_signer, connector, pool, crontab), _connector(connector), _pool(pool), _crontab(crontab)
    {
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

    // Methods

    void brahms :: lock()
    {
        this->_mutex.lock();
    }

    void brahms :: unlock()
    {
        this->_mutex.unlock();
    }

    // Private methods

    void brahms :: dispatch(const signature :: publickey & identifier)
    {
        for(size_t i = 0; i < settings :: sample :: size; i++)
            this->_sample[i].next(identifier);
    }
};
