// Forward declarations

namespace vine
{
    namespace dialers
    {
        class local
    };
};

#if !defined(__forward__) && !defined(__vine__dialers__local__h)
#define __vine__dialers__local__h

// Libraries

#include <unordered_map>

// Includes

#include "vine/network/dial.h"
#include "drop/crypto/hash.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/crypto/signature.hpp"
#include "drop/network/sockets/local.hpp"

namespace vine :: dialers
{
    using namespace drop;
};

#endif
