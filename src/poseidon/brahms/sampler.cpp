// Includes

#include "sampler.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    sampler :: sampler()
    {
        this->init();
    }

    // Getters

    const optional <identifier> & sampler :: sample() const
    {
        return this->_sample;
    }

    // Methods

    void sampler :: init()
    {
        this->_key = hash :: key :: random();
        this->_sample = null;
    }

    void sampler :: next(const identifier & identifier)
    {
        if(!(this->_sample))
        {
            this->_sample = identifier;
            this->_hash = hash :: keyed(this->_key, identifier);
        }
        else
        {
            hash idhash = hash :: keyed(this->_key, identifier);
            if(idhash < this->_hash)
            {
                this->_sample = identifier;
                this->_hash = idhash;
            }
        }
    }
};
