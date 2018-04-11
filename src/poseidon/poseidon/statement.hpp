#ifndef __poseidon__poseidon__statement__hpp
#define __poseidon__poseidon__statement__hpp

// Includes

#include "statement.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Methods

    template <typename vtype> void statement :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_identifier);
        reader << (this->_sequence);
        reader << (this->_value);
        reader << (this->_signature);
    }

    template <typename vtype> void statement :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_identifier);
        writer >> (this->_sequence);
        writer >> (this->_value);
        writer >> (this->_signature);
    }
};

#endif
