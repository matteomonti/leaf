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

    template <typename... types> template <typename needle, typename haywire, typename... haystack> constexpr ssize_t variant_base <types...> :: traits :: typeid_of()
    {
        if constexpr (std :: is_same <needle, haywire> :: value)
            return 0;

        if constexpr (sizeof...(haystack) == 0)
            return -1;
        else
        {
            ssize_t id = typeid_of <needle, haystack...> ();
            return (id == -1) ? -1 : id + 1;
        }
    }

    template <typename... types> template <typename vtype, typename... vtypes> constexpr bool variant_base <types...> :: traits :: distinct()
    {
        if constexpr (sizeof...(vtypes) == 0)
            return true;
        else
            return (typeid_of <vtype, vtypes...> () == -1) && distinct <vtypes...> ();
    }

    // Constraints

    template <typename... types> constexpr bool variant_base <types...> :: constraints :: variants()
    {
        if constexpr (sizeof...(types) == 0)
            return false;
        else
            return traits :: template distinct <types...> ();
    }

    template <typename... types> template <typename vtype> constexpr bool variant_base <types...> :: constraints :: variant()
    {
        return (traits :: template typeid_of <vtype, types...> () != -1);
    }

    template <typename... types> template <typename ctype> constexpr bool variant_base <types...> :: constraints :: callback()
    {
        return (... || (traits :: template callable <ctype, types> ()));
    }

    // Private constructors

    template <typename... types> template <typename vtype> variant_base <types...> :: variant_base(const vtype & value) : _typeid(traits :: template typeid_of <vtype, types...> ())
    {
        new (&(this->_value)) vtype(value);
    }

    // Private methods

    template <typename... types> template <size_t index, typename vtype, typename... vtypes> auto variant_base <types...> :: valueptr()
    {
        if constexpr (index == 0)
            return &(reinterpret_cast <vtype &> (this->_value));
        else
            return valueptr <index - 1, vtypes...> ();
    }

    template <typename... types> template <size_t index, typename vtype, typename... vtypes> const auto variant_base <types...> :: valueptr() const
    {
        if constexpr (index == 0)
            return &(reinterpret_cast <vtype &> (this->_value));
        else
            return valueptr <index - 1, vtypes...> ();
    }

    // variant

    // Private constructors

    template <typename... types> variant <types...> :: variant()
    {
    }

    // Constructors

    template <typename... types> template <typename vtype, std :: enable_if_t <variant <types...> :: constraints :: template variant <vtype> ()  && std :: is_copy_constructible <vtype> :: value> *> variant <types...> :: variant(const vtype & value) : variant_base <types...> (value)
    {
    }
};

#endif
