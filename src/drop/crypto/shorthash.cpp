// Includes

#include "shorthash.hpp"

namespace drop
{
    // Constructors

    shorthash :: shorthash()
    {
        crypto_shorthash_keygen(this->_key);
    }

    // Operators

    size_t shorthash :: operator () (const buffer & target) const noexcept
    {
        uint64_t hash;
        crypto_shorthash((uint8_t *) &hash, target, target.size(), this->_key);
        return hash;
    }
};
