#ifndef __drop__crypto__sha256__hpp
#define __drop__crypto__sha256__hpp

// Includes

#include "sha256.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // digest

    // Constructors

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, sha256> ()))> *> sha256 :: sha256(const atypes & ... acceptors) : sha256()
    {
        this->update(acceptors...);
    }

    // Methods

    template <typename vtype> void sha256 :: digest :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->_bytes;
    }

    template <typename vtype> void sha256 :: digest :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> this->_bytes;
    }

    // sha256

    // Methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, sha256> ()))> *> void sha256 :: update(const atypes & ... acceptors)
    {
        bytewise :: reader <sha256> reader(*this);
        (reader << ... << acceptors);
    }
};

#endif
