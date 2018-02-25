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
            return sfinaes :: template call_operator_accepts_reference <std :: remove_reference_t <ctype>, vtype> :: value;
        else
            return sfinaes :: template call_operator_accepts_reference <std :: remove_reference_t <ctype>, vtype> :: value || sfinaes :: template call_operator_accepts_reference <ctype, const vtype> :: value;
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

    template <typename... types> constexpr bool variant_base <types...> :: constraints :: valid_types()
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

    template <typename... types> template <typename vtype> constexpr bool variant_base <types...> :: constraints :: mutable_visitor()
    {
        return (... && (traits :: template is_callable <vtype, types> ()));
    }

    template <typename... types> template <typename vtype> constexpr bool variant_base <types...> :: constraints :: const_visitor()
    {
        return (... && (traits :: template is_callable <vtype, const types> ()));
    }

    template <typename... types> template <typename ctype> constexpr bool variant_base <types...> :: constraints :: mutable_case()
    {
        return (... || (traits :: template is_callable <ctype, types> ()));
    }

    template <typename... types> template <typename ctype> constexpr bool variant_base <types...> :: constraints :: const_case()
    {
        return (... || (traits :: template is_callable <ctype, const types> ()));
    }

    // Private constructors

    template <typename... types> variant_base <types...> :: variant_base()
    {
    }

    template <typename... types> template <typename vtype> variant_base <types...> :: variant_base(const vtype & value) : _typeid(traits :: template typeid_of <vtype, types...> ())
    {
        new (&(this->_value)) vtype(value);
    }

    // Methods

    template <typename... types> template <typename lambda, std :: enable_if_t <variant_base <types...> :: constraints :: template mutable_visitor <lambda> ()> *> void variant_base <types...> :: visit(lambda && callback)
    {
        this->unwrap <types...> (this->_typeid, callback);
    }

    template <typename... types> template <typename lambda, std :: enable_if_t <variant_base <types...> :: constraints :: template const_visitor <lambda> ()> *> void variant_base <types...> :: visit(lambda && callback) const
    {
        return this->unwrap <types...> (this->_typeid, callback);
    }

    template <typename... types> template <typename... lambdas, std :: enable_if_t <(... && (variant_base <types...> :: constraints :: template mutable_case <lambdas> ()))> *> void variant_base <types...> :: match(lambdas && ... callbacks)
    {
        this->visit([&](auto && variant)
        {
            if constexpr ((... || (traits :: template is_directly_callable <lambdas, decltype(variant)> ())))
                this->direct_dispatch(variant, callbacks...);
            else
                this->dispatch(variant, callbacks...);
        });
    }

    template <typename... types> template <typename... lambdas, std :: enable_if_t <(... && (variant_base <types...> :: constraints :: template mutable_case <lambdas> ()))> *> void variant_base <types...> :: match(lambdas && ... callbacks) const
    {
        this->visit([&](auto && variant)
        {
            if constexpr ((... || (traits :: template is_directly_callable <lambdas, decltype(variant)> ())))
                this->direct_dispatch(variant, callbacks...);
            else
                this->dispatch(variant, callbacks...);
        });
    }

    // Private methods

    template <typename... types> template <typename vtype, typename... vtypes, typename lambda> void variant_base <types...> :: unwrap(const size_t & index, lambda && callback)
    {
        if constexpr (sizeof...(vtypes) == 0)
            callback(reinterpret_cast <vtype &> (this->_value));
        else
        {
            if(index == 0)
                callback(reinterpret_cast <vtype &> (this->_value));
            else
                this->unwrap <vtypes...> (index - 1, callback);
        }
    }

    template <typename... types> template <typename vtype, typename... vtypes, typename lambda> void variant_base <types...> :: unwrap(const size_t & index, lambda && callback) const
    {
        if constexpr (sizeof...(vtypes) == 0)
            callback(reinterpret_cast <const vtype &> (this->_value));
        else
        {
            if(index == 0)
                callback(reinterpret_cast <const vtype &> (this->_value));
            else
                this->unwrap <vtypes...> (index - 1, callback);
        }
    }

    template <typename... types> template <typename vtype, typename lambda, typename... lambdas> void variant_base <types...> :: direct_dispatch(vtype & variant, lambda && callback, lambdas && ... callbacks)
    {
        if constexpr (traits :: template is_directly_callable <lambda, vtype> ())
            callback(variant);
        else if constexpr (sizeof...(callbacks) > 0)
            this->direct_dispatch(variant, callbacks...);

    }

    template <typename... types> template <typename vtype, typename lambda, typename... lambdas> void variant_base <types...> :: direct_dispatch(const vtype & variant, lambda && callback, lambdas && ... callbacks) const
    {
        if constexpr (traits :: template is_directly_callable <lambda, const vtype> ())
            callback(variant);
        else if constexpr (sizeof...(callbacks) > 0)
            this->direct_dispatch(variant, callbacks...);

    }

    template <typename... types> template <typename vtype, typename lambda, typename... lambdas> void variant_base <types...> :: dispatch(vtype & variant, lambda && callback, lambdas && ... callbacks)
    {
        if constexpr (traits :: template is_callable <lambda, vtype> ())
            callback(variant);
        else if constexpr (sizeof...(callbacks) > 0)
            this->dispatch(variant, callbacks...);

    }

    template <typename... types> template <typename vtype, typename lambda, typename... lambdas> void variant_base <types...> :: dispatch(const vtype & variant, lambda && callback, lambdas && ... callbacks) const
    {
        if constexpr (traits :: template is_callable <lambda, const vtype> ())
            callback(variant);
        else if constexpr (sizeof...(callbacks) > 0)
            this->dispatch(variant, callbacks...);

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

    // Static methods

    template <typename... types> template <typename vtype, typename... atypes, std :: enable_if_t <variant <types...> :: constraints :: template variant <vtype> () && std :: is_constructible <vtype, atypes...> :: value> *> variant <types...> variant <types...> :: construct(atypes && ... args)
    {
        variant <types...> variant;
        variant._typeid = traits :: template typeid_of <vtype, types...> ();
        new (&(variant._value)) vtype(args...);
        return variant;
    }
};

#endif
