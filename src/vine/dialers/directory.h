// Forward declarations

namespace vine
{
    namespace dialers
    {
        class directory;
    };
};

#if !defined(__forward__) && !defined(__vine__dialers__directory__h)
#define __vine__dialers__directory__h

// Includes

#include "drop/network/address.hpp"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/crypto/keyexchanger.h"
#include "drop/crypto/signature.hpp"
#include "drop/crypto/hash.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/chrono/time.hpp"
#include "drop/chrono/crontab.h"
#include "drop/utils/sfinae.hpp"
#include "vine/network/dial.h"

namespace vine :: dialers
{
    using namespace drop;

    class directory
    {
        // Signatures

        struct signatures
        {
            static const hash entry;
        };
    };
}

#endif
