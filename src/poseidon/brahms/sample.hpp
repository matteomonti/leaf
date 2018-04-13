#ifndef __poseidon__brahms__sample__hpp
#define __poseidon__brahms__sample__hpp

// Includes

#include "sample.h"

namespace poseidon
{
    using namespace vine;

    // Methods

    template <size_t size> template <typename alambda, typename rlambda> void sample <size> :: update(const identifier & identifier, const alambda & add, const rlambda & remove)
    {
        for(size_t i = 0; i < size; i++)
        {
            optional <vine :: identifier> previous = this->_samplers[i].sample();

            if(this->_samplers[i].next(identifier))
            {
                if(previous && std :: none_of(this->_samplers.begin(), this->_samplers.end(), [&](const sampler & sampler) {return sampler.sample() && (*(sampler.sample())) == (*previous);}))
                    remove(*previous);

                if(std :: count_if(this->_samplers.begin(), this->_samplers.end(), [&](const sampler & sampler) {return sampler.sample() && (*(sampler.sample())) == identifier;}) == 1)
                    add(identifier);
            }
        }
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

    // Operators

    template <size_t size> const sampler & sample <size> :: operator [] (const size_t & index) const
    {
        return this->_samplers[index];
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
