// Includes

#include "poseidon.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _gossiper(this->_signer.publickey(), (*this), crontab, log), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    // Methods

    void poseidon :: start()
    {
        this->_crawler.start();
    }

    void poseidon :: publish(const uint64_t & sequence, const buffer & value)
    {
        this->_gossiper.publish(statement(this->_signer, sequence, value));
    }

    // Private methods

    void poseidon :: gossip(const statement & statement)
    {
        this->emit <events :: gossip> (statement);

        this->_mutex.lock();

        if(!(this->_checkpool.find(statement.index())))
        {
            this->_checkpool.add(statement.index());
        }

        this->_mutex.unlock();
    }
};
