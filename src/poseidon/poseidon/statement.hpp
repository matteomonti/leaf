#ifndef __poseidon__poseidon__statement__hpp
#define __poseidon__poseidon__statement__hpp

// Includes

#include "statement.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // index

    // Methods

    template <typename vtype> void index :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_identifier);
        reader << (this->_sequence);
    }

    template <typename vtype> void index :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_identifier);
        writer >> (this->_sequence);
    }

    // value

    // Methods

    template <typename vtype> void value :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->value;
        reader << this->signature;
    }

    template <typename vtype> void value :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> this->value;
        writer >> this->signature;
    }

    // statement

    // Methods

    template <typename vtype> void statement :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_index);
        reader << (this->_value);
        reader << (this->_signature);
    }

    template <typename vtype> void statement :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_index);
        writer >> (this->_value);
        writer >> (this->_signature);
    }
};

#endif
