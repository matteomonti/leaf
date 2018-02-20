#ifndef __drop__crypto__box__hpp
#define __drop__crypto__box__hpp

// Includes

#include "box.h"
#include "drop/data/tag.hpp"

namespace drop
{
    // Methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> *> buffer box :: encrypt(const class publickey & to, const atypes & ... acceptors)
    {
        return this->encrypt(to, bytewise :: serialize(acceptors...));
    }

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <atypes> ()))> *> auto box :: decrypt(const class publickey & from, const buffer & ciphertext)
    {
        return bytewise :: deserialize <atypes...> (this->decrypt(from, ciphertext));
    }
};

#endif
