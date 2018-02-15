#ifndef __drop__crypto__secretbox__hpp
#define __drop__crypto__secretbox__hpp

// Includes

#include "secretbox.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"

namespace drop
{
    // key

    // Methods

    template <typename vtype> void secretbox :: key :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void secretbox :: key :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // nonce

    // Methods

    template <typename vtype> void secretbox :: nonce :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void secretbox :: nonce :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // secretbox

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> *> buffer secretbox :: encrypt(const atypes & ... acceptors)
    {
        return this->encrypt(bytewise :: serialize(acceptors...));
    }

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <atypes> ()))> *> auto secretbox :: decrypt(const buffer & ciphertext)
    {
        return bytewise :: deserialize <atypes...> (this->decrypt(ciphertext));
    }
};

#endif
