#ifndef __poseidon__poseidon__poseidon__hpp
#define __poseidon__poseidon__poseidon__hpp

// Includes

#include "poseidon.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

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
