#ifndef __drop__crypto__hash__hpp
#define __drop__crypto__hash__hpp

// Includes

#include "hash.h"

namespace drop
{
    // hash

    // Methods

    template <typename vtype> void hash :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void hash :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // hasher

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> void hasher :: update(const atypes & ... acceptors)
    {
        bytewise :: reader <hasher> reader(*this);
        (reader << ... << acceptors);
    }
};

#endif
