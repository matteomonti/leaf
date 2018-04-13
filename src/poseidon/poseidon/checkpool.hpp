#ifndef __poseidon__poseidon__checkpool__hpp
#define __poseidon__poseidon__checkpool__hpp

// Includes

#include "checkpool.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Methods

    template <size_t size> size_t checkpool <size> :: init(const std :: unordered_set <index, shorthash> & indexes)
    {
        this->_version++;

        this->_indexes.clear();
        this->_indexes.reserve(indexes.size());

        for(size_t i = 0; i < size; i++)
        {
            this->_slots[i].clear();
            this->_slots[i].reserve(indexes.size());
        }

        for(const index & index : indexes)
            this->_indexes.push_back(index);

        return this->_version;
    }

    template <size_t size> void checkpool <size> :: push(const size_t & version, const size_t & slot, const optional <buffer> & value)
    {
        if(version != this->_version)
            return;

        this->_slots[slot].push_back(value);
    }
};

#endif
