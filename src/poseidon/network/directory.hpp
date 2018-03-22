#ifndef __poseidon__network__directory__hpp
#define __poseidon__network__directory__hpp

// Includes

#include "directory.h"

namespace poseidon
{
    using namespace drop;

    // client

    // Methods

    template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, connection> :: value) && (acceptors :: tcp :: async :: constraints :: callback <lambda> ())> *> void directory :: client :: on(const lambda & handler)
    {
        this->_acceptor.on <connection> (handler);
    }
};

#endif
