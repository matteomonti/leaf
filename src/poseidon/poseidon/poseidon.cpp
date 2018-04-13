// Includes

#include "poseidon.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // index

    // Operators

    bool poseidon :: index :: operator == (const index & rho) const
    {
        return (this->identifier == rho.identifier) && (this->sequence == rho.sequence);
    }

    // poseidon

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _gossiper(this->_signer.publickey(), (*this), crontab), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _sequence(0), _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }

    // Methods

    void poseidon :: start()
    {
        this->_crawler.start();
    }

    void poseidon :: publish(const buffer & value)
    {
        this->_mutex.lock();

        class statement statement(this->_signer, this->_sequence, value);
        this->_gossiper.add(statement);
        this->_sequence++;

        this->_mutex.unlock();
    }

    // Private methods

    void poseidon :: gossip(const statement & statement)
    {
        index index{.identifier = statement.identifier(), .sequence = statement.sequence()};

        this->_mutex.lock();

        try
        {
            this->_logs.at(index);
        }
        catch(...)
        {
            this->_logs[index] = entry{.value = statement.value(), .signature = statement.signature(), .timestamp = now, .accepted = false};
            this->_checklist.insert(index);
        }

        this->_mutex.unlock();
    }
};
