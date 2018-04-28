// Includes

#include "sampler.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    sampler :: sampler()
    {
    }

    // Getters

    const identifier & sampler :: sample() const
    {
        return this->_sample;
    }

    // Methods

    void sampler :: init(const std :: array <identifier, settings :: view :: size> & view)
    {
        this->_key = hash :: key :: random();

        this->_sample = view[0];
        for(size_t index = 1; index < settings :: view :: size; index++)
            this->next(view[index]);
    }

    void sampler :: next(const identifier & identifier)
    {
        hash idhash = hash :: keyed(this->_key, identifier);
        if(idhash < this->_hash)
        {
            this->_sample = identifier;
            this->_hash = idhash;
        }
    }
};
