#ifndef __drop__data__variant__hpp
#define __drop__data__variant__hpp

// Includes

#include "variant.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/utils/sfinae.hpp"

namespace drop
{
    // variant_base

    // Traits

    template <typename... types> template <typename ctype, typename vtype> constexpr bool variant_base <types...> :: traits :: is_callable()
    {
        return sfinae :: returns <void> ([](auto && callback) -> decltype(callback(std :: declval <vtype &> ())) {}).template satisfied <ctype> ();
    }

    template <typename... types> template <typename ctype, typename vtype> constexpr bool variant_base <types...> :: traits :: is_directly_callable()
    {
        if constexpr (std :: is_const <vtype> :: value)
            return sfinaes :: template call_operator_accepts_reference <ctype, vtype> :: value;
        else
            return sfinaes :: template call_operator_accepts_reference <ctype, vtype> :: value || sfinaes :: template call_operator_accepts_reference <ctype, const vtype> :: value;
    }

    template <typename... types> template <typename needle, typename haywire, typename... haystack> constexpr bool variant_base <types...> :: traits :: in()
    {
        if constexpr (sizeof...(haystack) == 0)
            return std :: is_same <needle, haywire> :: value;
        else
            return std :: is_same <needle, haywire> :: value || in <needle, haystack...> ();
    }

    template <typename... types> template <typename vtype, typename... vtypes> constexpr bool variant_base <types...> :: traits :: distinct()
    {
        if constexpr (sizeof...(vtypes) == 0)
            return true;
        else
            return !(in <vtype, vtypes...> ()) && distinct <vtypes...> ();
    }

    // Constraints

    template <typename... types> template <typename ctype> constexpr bool variant_base <types...> :: constraints :: callback()
    {
        return (... || (traits :: template callable <ctype, types> ()));
    }

    template <typename... types> constexpr bool variant_base <types...> :: constraints :: variants()
    {
        if constexpr (sizeof...(types) == 0)
            return false;
        else
            return traits :: template distinct <types...> ();
    }

    // Constructors

    template <typename... types> variant_base <types...> :: variant_base()
    {
    }
};

#endif
