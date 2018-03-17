// Includes

#include "shorthash.hpp"

namespace drop
{
    // Constructors

    shorthash :: shorthash()
    {
        crypto_shorthash_keygen(this->_key);
    }
};
