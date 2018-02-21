// Forward declarations

namespace drop
{
    template <typename...> class variant;
};

#if !defined(__forward__) && !defined(__drop__data__variant__h)
#define __drop__data__variant__h

// Includes

#include "drop/bytewise/bytewise.h"
#include "drop/utils/sfinae.h"
#include "drop/utils/static_math.hpp"

namespace drop
{
    template <typename... types> class variant
    {
        // Traits

        struct traits
        {
            template <typename ctype, typename vtype> static constexpr bool callable();
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename ctype> static constexpr bool callback();
        };

    private:

        // Members

        uint8_t _variant;
        std :: aligned_storage_t <max({sizeof(types)...}), max({alignof(types)...})> _value;
    };
};

#endif
