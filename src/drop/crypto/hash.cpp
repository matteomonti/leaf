// Includes

#include "hash.hpp"

namespace drop
{
    // key

    // Static methods

    hash :: key hash :: key :: random()
    {
        key key;
        randombytes_buf(key._bytes, size);
        return key;
    }

    // hash

    // Private constructors

    hash :: hash(crypto_generichash_state & state)
    {
        crypto_generichash_final(&state, this->_bytes, size);
    }

    // hasher

    // Constructors

    hasher :: hasher()
    {
        crypto_generichash_init(&(this->_state), nullptr, 0, hash :: size);
    }

    hasher :: hasher(const hash :: key & key)
    {
        crypto_generichash_init(&(this->_state), key, hash :: key :: size, hash :: size);
    }

    // Methods

    hasher & hasher :: update(const uint8_t * bytes, const size_t & size)
    {
        crypto_generichash_update(&(this->_state), bytes, size);
        return (*this);
    }

    hash hasher :: finalize()
    {
        return hash(this->_state);
    }
};
