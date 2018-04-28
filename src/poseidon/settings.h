// Forward declarations

namespace poseidon
{
    struct settings;
};

#if !defined(__forward__) && !defined(__poseidon__settings__h)
#define __poseidon__settings__h

// Includes

#include <stddef.h>

namespace poseidon
{
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
    };
};

#endif
