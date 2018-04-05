#ifndef __poseidon__brahms__pullpool__hpp
#define __poseidon__brahms__pullpool__hpp

// Includes

#include "pullpool.h"

namespace poseidon
{
    using namespace vine;
    using namespace drop;

    // Constructors

    template <size_t psize, size_t vsize> pullpool <psize, vsize> :: pullpool(const identifier & identifier) : _identifier(identifier)
    {
    }

    // Methods

    template <size_t psize, size_t vsize> size_t pullpool <psize, vsize> :: init()
    {
        this->_version++;

        for(size_t i = 0; i < psize; i++)
            this->_slots[i] = null;

        return this->_version;
    }

    template <size_t psize, size_t vsize> void pullpool <psize, vsize> :: set(const size_t & version, const size_t & slot, const std :: array <identifier, vsize> & view)
    {
        if(version == this->_version)
            this->_slots[slot] = view;
    }

    template <size_t psize, size_t vsize> const identifier & pullpool <psize, vsize> :: random()
    {
        while(true)
        {
            size_t slot = randombytes_uniform(psize);

            if(!(this->_slots[slot]))
                continue;

            size_t identifier = randombytes_uniform(vsize);

            if((*(this->_slots[slot]))[identifier] != this->_identifier)
                return (*(this->_slots[slot]))[identifier];
        }
    }

    // Casting

    template <size_t psize, size_t vsize> pullpool <psize, vsize> :: operator bool ()
    {
        for(size_t i = 0; i < psize; i++)
            if(this->_slots[i])
                for(size_t j = 0; j < vsize; j++)
                    if((*(this->_slots[i]))[j] != this->_identifier)
                        return true;

        return false;
    }
};

#endif
