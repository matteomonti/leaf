#ifndef __drop__crypto__hash__hpp
#define __drop__crypto__hash__hpp

// Includes

#include "hash.h"

namespace drop
{
    // key

    // Methods

    template <typename vtype> void hash :: key :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void hash :: key :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // hash

    // Constructors

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> hash :: hash(const atypes & ... acceptors)
    {
        hasher hasher;
        hasher.update(acceptors...);
        (*this) = hasher.finalize();
    }

    // Methods

    template <typename vtype> void hash :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void hash :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // Static methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> hash hash :: keyed(const key & key, const atypes & ... acceptors)
    {
        hasher hasher(key);
        hasher.update(acceptors...);
        return hasher.finalize();
    }

    // hasher

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> void hasher :: update(const atypes & ... acceptors)
    {
        bytewise :: reader <hasher> reader(*this);
        (reader << ... << acceptors);
    }
};

#endif
