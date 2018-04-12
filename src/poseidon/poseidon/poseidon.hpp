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
};

#endif
