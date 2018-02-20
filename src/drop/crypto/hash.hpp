#ifndef __drop__crypto__hash__hpp
#define __drop__crypto__hash__hpp

// Includes

#include "hash.h"
#include "drop/data/tag.hpp"

namespace drop
{
    // hash

    // Constructors

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> hash :: hash(const atypes & ... acceptors)
    {
        hasher hasher;
        hasher.update(acceptors...);
        (*this) = hasher.finalize();
    }

    // Static methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> hash hash :: keyed(const key & key, const atypes & ... acceptors)
    {
        return hasher(key).update(acceptors...).finalize();
    }

    // hasher

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> *> hasher & hasher :: update(const atypes & ... acceptors)
    {
        bytewise :: reader <hasher> reader(*this);
        (reader << ... << acceptors);
        return (*this);
    }
};

#endif
