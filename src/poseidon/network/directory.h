// Forward declarations

namespace poseidon
{
    class directory;
};

#if !defined(__forward__) && !defined(__poseidon__network__directory__h)
#define __poseidon__network__directory__h

// Includes

#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.h"
#include "drop/crypto/keyexchanger.h"
#include "drop/crypto/signature.hpp"

namespace poseidon
{
    using namespace drop;

    class directory
    {
    public:

        // Nested classes

        class server
        {
        };

        class client
        {
        };
    };
};

#endif
