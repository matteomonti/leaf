#ifndef __poseidon__brahms__pushpool__hpp
#define __poseidon__brahms__pushpool__hpp

// Includes

#include "pushpool.h"

namespace poseidon
{
    using namespace vine;
    
    // Methods

    template <size_t size> void pushpool <size> :: init()
    {
        this->_slots.clear();
    }

    template <size_t size> void pushpool <size> :: push(const identifier & identifier)
    {
        this->_slots.push_back(identifier);
    }

    template <size_t size> const identifier & pushpool <size> :: random()
    {
        return this->_slots[randombytes_uniform(this->_slots.size())];
    }

    // Casting

    template <size_t size> pushpool <size> :: operator bool ()
    {
        return (this->_slots.size() > 0) && (this->_slots.size() <= size);
    }
};

#endif
