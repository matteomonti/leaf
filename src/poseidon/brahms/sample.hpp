#ifndef __poseidon__brahms__sample__hpp
#define __poseidon__brahms__sample__hpp

// Includes

#include "sample.h"

namespace poseidon
{
    using namespace vine;

    // Methods

    template <size_t size> void sample <size> :: update(const identifier & identifier)
    {
        for(size_t i = 0; i < size; i++)
            this->_samplers[i].next(identifier);
    }

    template <size_t size> const identifier & sample <size> :: random()
    {
        while(true)
        {
            size_t sampler = randombytes_uniform(size);
            if(this->_samplers[sampler].sample())
                return *(this->_samplers[sampler].sample());
        }
    }

    // Casting

    template <size_t size> sample <size> :: operator bool ()
    {
        for(size_t i = 0; i < size; i++)
            if(this->_samplers[i].sample())
                return true;

        return false;
    }
};

#endif
