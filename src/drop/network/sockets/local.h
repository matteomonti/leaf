// Forward declarations

namespace drop
{
    namespace sockets
    {
        class local;
    };
};

#if !defined(__forward__) && !defined(__drop__network__sockets__local__h)
#define __drop__network__sockets__local__h

// Libraries

#include <sys/types.h>
#include <sys/socket.h>

// Includes

#include "exceptions.h"
#include "drop/chrono/time.hpp"
#include "drop/utils/sfinae.h"

namespace drop :: sockets
{
    class local
    {
    };
};

#endif
