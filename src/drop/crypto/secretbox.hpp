#ifndef __drop__crypto__secretbox__hpp
#define __drop__crypto__secretbox__hpp

// Includes

#include "secretbox.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"
#include "drop/data/tag.hpp"

namespace drop
{
    // secretbox

    // Methods

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
