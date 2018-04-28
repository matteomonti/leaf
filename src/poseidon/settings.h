// Forward declarations

namespace poseidon
{
    struct settings;
};

#if !defined(__forward__) && !defined(__poseidon__settings__h)
#define __poseidon__settings__h

// Libraries

#include <stddef.h>

// Includes

#include "drop/chrono/time.hpp"

namespace poseidon
{
    using namespace drop;

    struct settings
    {
        struct sample
        {
            static constexpr size_t size = 16;
        };

        struct view
        {
            static constexpr size_t size = 8;
        };

        static constexpr size_t channels = 3;

        struct brahms
        {
            static constexpr interval interval = 10_s;

            static constexpr size_t alpha = 3;
            static constexpr size_t beta = 3;
            static constexpr size_t gamma = 2;
        };
    };
};

#endif
