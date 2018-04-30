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
            static constexpr size_t size = 8;
        };

        struct view
        {
            static constexpr size_t size = 6;
        };

        static constexpr size_t channels = 3;

        struct brahms
        {
            static constexpr interval interval = 30_s;

            static constexpr size_t alpha = 2;
            static constexpr size_t beta = 2;
            static constexpr size_t gamma = 2;

            static_assert(alpha + beta + gamma == view :: size, "alpha + beta + gamma must be equal to the view size!");
        };

        struct gossiper
        {
            struct intervals
            {
                static constexpr interval gossip = 10_s;
                static constexpr interval reward = 8_s;
                static constexpr interval ban = 30_s;
            };

            struct thresholds
            {
                static constexpr int32_t cap = 4;
                static constexpr int32_t push = 2;
                static constexpr int32_t ban = -2;
            };
        };

        struct poseidon
        {
            struct thresholds
            {
                static constexpr size_t quorum = sample :: size; // TODO: Maybe change this in attack scenarios! Only for the sake of the experiment.
                static constexpr size_t accept = sample :: size;
            };
        };
    };
};

#endif
