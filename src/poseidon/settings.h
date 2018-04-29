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
            static constexpr interval interval = 5_m;

            static constexpr size_t alpha = 3;
            static constexpr size_t beta = 3;
            static constexpr size_t gamma = 2;
        };

        struct gossiper
        {
            struct intervals
            {
                static constexpr interval gossip = 1_m;
                static constexpr interval reward = 50_s;
                static constexpr interval ban = 2_m;
            };

            struct thresholds
            {
                static constexpr int32_t push = 2;
                static constexpr int32_t ban = -4;
            };
        };
    };
};

#endif
