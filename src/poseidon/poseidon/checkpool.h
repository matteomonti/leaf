// Forward declarations

namespace poseidon
{
    template <size_t> class checkpool;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__checkpool__h)
#define __poseidon__poseidon__checkpool__h

// Libraries

#include <vector>
#include <unordered_set>

// Forward includes

#define __forward__
#include "poseidon.h"
#undef __forward__

// Includes

#include "statement.hpp"
#include "drop/data/buffer.hpp"
#include "drop/crypto/shorthash.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    template <size_t size> class checkpool
    {
        // Friends

        friend class poseidon;
    };
};

#endif
