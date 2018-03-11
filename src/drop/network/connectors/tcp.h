// Forward declarations

namespace drop
{
    namespace connectors
    {
        class tcp;
    };
};

#if !defined(__forward__) && !defined(__drop__network__connectors__h)
#define __drop__network__connectors__h

// Libraries

#include <thread>
#include <exception>

#include "drop/network/address.h"
#include "drop/network/sockets/tcp.hpp"
#include "drop/network/connection.hpp"

namespace drop :: connectors
{
    class tcp
    {
    public:

        // Nested classes

        class sync
        {
        public:

            // Static methods

            static connection connect(const address &);
        };
    };
};

#endif
