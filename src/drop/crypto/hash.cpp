// Includes

#include "hash.hpp"

namespace drop
{
    // hash

    // Private constructors

    hash :: hash(crypto_generichash_state & state)
    {
        crypto_generichash_final(&state, this->_bytes, size);
    }

    // Operators

    const uint8_t & hash :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    bool hash :: operator == (const hash & rho) const
    {
        return !memcmp(this->_bytes, rho._bytes, size); // TODO: Maybe use a constant-time comparison?
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const hash & hash)
    {
        out << "<";

        for(size_t i = 0; i < hash :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(hash[i]);

        out << ">";

        return out;
    }

    // hasher

    // Constructors

    hasher :: hasher()
    {
        crypto_generichash_init(&(this->_state), nullptr, 0, hash :: size);
    }

    // Methods

    void hasher :: update(const uint8_t * bytes, const size_t & size)
    {
        crypto_generichash_update(&(this->_state), bytes, size);
    }

    hash hasher :: finalize()
    {
        return hash(this->_state);
    }
};
