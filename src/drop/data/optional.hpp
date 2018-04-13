#ifndef __drop__data__optional__hpp
#define __drop__data__optional__hpp

// Includes

#include "optional.h"

namespace drop
{
    // optional_base

    // Constraints

    template <typename type> template <typename vtype> constexpr bool optional_base <type> :: constraints :: readable()
    {
        return bytewise :: constraints :: readable <type, vtype> ();
    }

    template <typename type> template <typename vtype> constexpr bool optional_base <type> :: constraints :: writable()
    {
        return bytewise :: constraints :: writable <type, vtype> () && std :: is_default_constructible <type> :: value;
    }

    // Constructors

    template <typename type> optional_base <type> :: optional_base() : _set(false)
    {
    }

    template <typename type> optional_base <type> :: optional_base(class def) : _set(true)
    {
        new (&(this->_value)) type();
    }

    template <typename type> optional_base <type> :: optional_base(class null) : _set(false)
    {
    }

    template <typename type> template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> *> optional_base <type> :: optional_base(types && ... args) : _set(true)
    {
        new (&(this->_value)) type(args...);
    }

    template <typename type> optional_base <type> :: optional_base(const optional_base & that) : _set(that._set)
    {
        if(this->_set)
            new (&(this->_value)) type(reinterpret_cast <const type &> (that._value));
    }

    template <typename type> optional_base <type> :: optional_base(optional_base && that) : _set(that._set)
    {
        if(this->_set)
            new (&(this->_value)) type(std :: move(reinterpret_cast <type &&> (that._value)));
    }

    // Destructor

    template <typename type> optional_base <type> :: ~optional_base()
    {
        if(this->_set)
            (reinterpret_cast <type &> (this->_value)).~type();
    }

    // Methods

    template <typename type> template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> *> void optional_base <type> :: emplace(types && ... args)
    {
        this->~optional_base();

        this->_set = true;
        new (&(this->_value)) type(args...);
    }

    template <typename type> template <typename vtype, std :: enable_if_t <optional_base <type> :: constraints :: template readable <vtype> ()> *> void optional_base <type> :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->_set;

        if(this->_set)
            reader << reinterpret_cast <const type &> (this->_value);
    }

    template <typename type> template <typename vtype, std :: enable_if_t <optional_base <type> :: constraints :: template writable <vtype> ()> *> void optional_base <type> :: accept(bytewise :: writer <vtype> & writer)
    {
        bool set;
        writer >> set;

        if(set)
        {
            if(!(this->_set))
                new (&(this->_value)) type();

            writer >> reinterpret_cast <type &> (this->_value);
        }
        else
            this->~optional_base();

        this->_set = set;
    }

    // Operators

    template <typename type> type & optional_base <type> :: operator * ()
    {
        return reinterpret_cast <type &> (this->_value);
    }

    template <typename type> const type & optional_base <type> :: operator * () const
    {
        return reinterpret_cast <const type &> (this->_value);
    }

    template <typename type> type * optional_base <type> :: operator -> ()
    {
        return &(reinterpret_cast <type &> (this->_value));
    }

    template <typename type> const type * optional_base <type> :: operator -> () const
    {
        return &(reinterpret_cast <const type &> (this->_value));
    }

    template <typename type> optional_base <type> & optional_base <type> :: operator = (class null)
    {
        if(this->_set)
        {
            this->_set = false;
            reinterpret_cast <type &> (this->_value).~type();
        }

        return (*this);
    }

    template <typename type> optional_base <type> & optional_base <type> :: operator = (const type & rho)
    {
        if(this->_set)
            reinterpret_cast <type &> (this->_value) = rho;
        else
        {
            this->_set = true;
            new (&(this->_value)) type(rho);
        }

        return (*this);
    }

    template <typename type> optional_base <type> & optional_base <type> :: operator = (type && rho)
    {
        if(this->_set)
            reinterpret_cast <type &> (this->_value) = std :: move(rho);
        else
        {
            this->_set = true;
            new (&(this->_value)) type(std :: move(rho));
        }

        return (*this);
    }

    template <typename type> optional_base <type> & optional_base <type> :: operator = (const optional_base <type> & that)
    {
        if(that._set)
            (*this) = (*that);
        else
            (*this) = null;

        return (*this);
    }

    template <typename type> optional_base <type> & optional_base <type> :: operator = (optional_base <type> && that)
    {
        if(that._set)
            (*this) = std :: move(*that);
        else
            (*this) = null;

        return (*this);
    }

    // Casting

    template <typename type> optional_base <type> :: operator const bool & () const
    {
        return this->_set;
    }

    // optional

    // Constructors

    template <typename type> template <typename dtype, std :: enable_if_t <std :: is_same <dtype, class def> :: value && std :: is_default_constructible <type> :: value> *> optional <type> :: optional(dtype) : optional_base <type> (def)
    {
    }

    template <typename type> optional <type> :: optional(class null) : optional_base <type> (null)
    {
    }

    template <typename type> template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> *> optional <type> :: optional(types && ... args) : optional_base <type> (args...)
    {
    }

    // Operators

    template <typename type> optional <type> & optional <type> :: operator = (class null)
    {
        ((optional_base <type> &)(*this)) = null;
        return (*this);
    }

    template <typename type> template <typename ttype, std :: enable_if_t <std :: is_same <ttype, type> :: value && std :: is_copy_assignable <type> :: value && std :: is_copy_constructible <type> :: value> *> optional <type> & optional <type> :: operator = (const ttype & rho)
    {
        ((optional_base <type> &)(*this)) = rho;
        return (*this);
    }

    template <typename type> template <typename ttype, std :: enable_if_t <std :: is_same <ttype, type> :: value && std :: is_move_assignable <type> :: value && std :: is_move_constructible <type> :: value> *> optional <type> & optional <type> :: operator = (ttype && rho)
    {
        ((optional_base <type> &)(*this)) = std :: move(rho);
        return (*this);
    }
};

#endif
