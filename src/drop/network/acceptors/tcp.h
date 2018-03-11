// Forward declarations

namespace drop
{
    namespace acceptors
    {
        class tcp;
    };
};

#if !defined(__forward__) && !defined(__drop__network__acceptors__tcp__h)
#define __drop__network__acceptors__tcp__h

// Libraries

#include <stdint.h>
#include <type_traits>
#include <thread>
#include <mutex>

// Includes

#include "drop/network/sockets/tcp.h"
#include "drop/network/connection.hpp"
#include "drop/network/connectors/tcp.h"

namespace drop :: acceptors
{
    class tcp
    {
    public:

        // Nested classes

        class sync
        {
            // Members

            sockets :: tcp _socket;

        public:

            // Constructors

            sync(const uint16_t &);

            // Methods

            connection accept();
        };
    };
};

#endif
