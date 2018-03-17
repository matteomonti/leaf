#ifndef __drop__crypto__shorthash__hpp
#define __drop__crypto__shorthash__hpp

// Includes

#include "shorthash.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"
#include "drop/data/tag.hpp"

namespace drop
{
    // Operators

    template <size_t length> size_t shorthash :: operator () (const tag <length> & target) const noexcept
    {
        uint64_t hash;
        crypto_shorthash((uint8_t *) &hash, target, length, this->_key);
        return hash;
    }

    template <typename type, std :: enable_if_t <bytewise :: constraints :: serializable <type> ()> *> size_t shorthash :: operator () (const type & target) const noexcept
    {
        buffer bytes = bytewise :: serialize(target);
        uint64_t hash;
        crypto_shorthash((uint8_t *) &hash, bytes, bytes.size(), this->_key);

        return hash;
    }
};

#endif
