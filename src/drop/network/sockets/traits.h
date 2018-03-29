// Forward declarations

namespace drop
{
    namespace sockets
    {
        struct traits;
        struct constraints;
    };
};

#if !defined(__forward__) && !defined(__drop__network__sockets__traits__h)
#define __drop__network__sockets__traits__h

// Includes

#include "drop/utils/sfinae.h"

namespace drop :: sockets
{
    // Traits

    struct traits
    {
    private:

        // Friends

        friend class tcp;
        friend class local;
        friend class constraints;

        // Static constexpressions

        template <typename stype> static constexpr bool has_update_method();
        template <typename stype> static constexpr bool has_pending_method();
    };

    struct constraints
    {
        template <typename stype> static constexpr bool streamer();
    };
};

#endif
