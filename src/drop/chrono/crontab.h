// Forward declarations

namespace drop
{
    class crontab;
};

#if !defined(__forward__) && !defined(__drop__chrono__crontab__h)
#define __drop__chrono__crontab__h

// Libraries

#include <thread>
#include <queue>

// Includes

#include "time.h"
#include "drop/async/promise.h"

namespace drop
{
    class crontab
    {
        // Settings

        struct settings
        {
            struct bucket
            {
                static constexpr interval size = 0.1_s;
                static constexpr size_t count = 3600;
            };
        };
    };
};

#endif
