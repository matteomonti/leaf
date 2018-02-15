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

    // nonce

    // Operators

    const uint8_t & secretbox :: nonce :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    secretbox :: nonce & secretbox :: nonce :: operator ++ ()
    {
        sodium_increment(this->_bytes, size);
        return (*this);
    }

    secretbox :: nonce & secretbox :: nonce :: operator ++ (int)
    {
        sodium_increment(this->_bytes, size);
        return (*this);
    }

    bool secretbox :: nonce :: operator == (const nonce & rho) const
    {
        return !(memcmp(this->_bytes, rho._bytes, size));
    }

    // Casting

    secretbox :: nonce :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // Static methods

    secretbox :: nonce secretbox :: nonce :: random()
    {
        nonce nonce;
        randombytes_buf(nonce._bytes, size);
        return nonce;
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

    std :: ostream & operator << (std :: ostream & out, const secretbox :: nonce & nonce)
    {
        out << "<";

        for(size_t i = 0; i < secretbox :: nonce :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(nonce[i]);

        out << ">";

        return out;
    }
};
