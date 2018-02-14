// Includes

#include "secretbox.hpp"

namespace drop
{
    // key

    // Operators

    const uint8_t & secretbox :: key :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    bool secretbox :: key :: operator == (const key & rho) const
    {
        return !(memcmp(this->_bytes, rho._bytes, size));
    }

    // Casting

    secretbox :: key :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // Static methods

    secretbox :: key secretbox :: key :: random()
    {
        key key;
        randombytes_buf(key._bytes, size);
        return key;
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const secretbox :: key & key)
    {
        out << "<";

        for(size_t i = 0; i < secretbox :: key :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(key[i]);

        out << ">";

        return out;
    }
};
