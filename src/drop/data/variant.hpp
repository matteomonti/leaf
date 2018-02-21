#ifndef __drop__data__variant__hpp
#define __drop__data__variant__hpp

// Includes

#include "variant.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/utils/sfinae.hpp"

namespace drop
{
    // Traits

    template <typename... types> template <typename ctype, typename vtype> constexpr bool variant <types...> :: traits :: is_callable()
    {
        return sfinae :: returns <void> ([](auto && callback) -> decltype(callback(std :: declval <vtype &> ())) {}).template satisfied <ctype> ();
    }

    template <typename... types> template <typename ctype, typename vtype> constexpr bool variant <types...> :: traits :: is_directly_callable()
    {
        if constexpr (std :: is_const <vtype> :: value)
            return sfinaes :: template call_operator_accepts_reference <ctype, vtype> :: value;
        else
            return sfinaes :: template call_operator_accepts_reference <ctype, vtype> :: value || sfinaes :: template call_operator_accepts_reference <ctype, const vtype> :: value;
    }

    // Constraints

    template <typename... types> template <typename ctype> constexpr bool variant <types...> :: constraints :: callback()
    {
        return (... || (traits :: template callable <ctype, types> ()));
    }
};

#endif
