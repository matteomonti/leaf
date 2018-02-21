#ifndef __drop__data__variant__hpp
#define __drop__data__variant__hpp

// Includes

#include "variant.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/utils/sfinae.hpp"

namespace drop
{
    // Traits

    template <typename... types> template <typename ctype, typename vtype> constexpr bool variant <types...> :: traits :: callable()
    {
        return sfinae :: is_valid([](auto && callback) -> decltype(callback(std :: declval <vtype &> ())) {}).template satisfied <ctype> ();
    }

    // Constraints

    template <typename... types> template <typename ctype> constexpr bool variant <types...> :: constraints :: callback()
    {
        return (... || (traits :: template callable <ctype, types> ()));
    }
};

#endif
