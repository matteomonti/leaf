#ifndef __drop__crypto__signature__hpp
#define __drop__crypto__signature__hpp

// Includes

#include "signature.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/tag.hpp"

namespace drop
{
    // signer

    // Methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> *> signature signer :: sign(const atypes & ... acceptors)
    {
        return this->sign(bytewise :: serialize(acceptors...));
    }

    // verifier

    // Methods

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> *> void verifier :: verify(const signature & signature, const atypes & ... acceptors)
    {
        this->verify(signature, bytewise :: serialize(acceptors...));
    }
};

#endif
