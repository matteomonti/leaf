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

        for(const index & index : indexes)
            this->_indexes.push_back(index);

        return this->_version;
    }

    template <size_t size> void checkpool <size> :: set(const size_t & version, const size_t & slot, const std :: vector <optional <buffer>> & responses)
    {
        if(version != this->_version)
            return;

        this->_slots[slot] = responses;
    }
};

#endif
