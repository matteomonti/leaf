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
            static constexpr size_t size = 32;
        };

        struct view
        {
            static constexpr size_t size = 16;
        };

        static constexpr size_t channels = 3;

        struct brahms
        {
            static constexpr interval interval = 1_m;

            static constexpr size_t alpha = 6;
            static constexpr size_t beta = 6;
            static constexpr size_t gamma = 4;
        };
    };
};

#endif
