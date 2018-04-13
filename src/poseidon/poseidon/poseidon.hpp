#ifndef __poseidon__poseidon__poseidon__hpp
#define __poseidon__poseidon__poseidon__hpp

// Includes

#include "poseidon.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // index

    // Methods

    template <typename vtype> void poseidon :: index :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->identifier;
        reader << this->sequence;
    }

    template <typename vtype> void poseidon :: index :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> this->identifier;
        writer >> this->sequence;
    }

    // value

    template <typename vtype> void poseidon :: value :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->value;
        reader << this->signature;
    }

    template <typename vtype> void poseidon :: value :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> this->value;
        writer >> this->signature;
    }
};

#endif
