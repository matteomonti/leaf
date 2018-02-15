// Includes

#include "box.h"

namespace drop
{
    // publickey

    // Operators

    const uint8_t & box :: publickey :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    bool box :: publickey :: operator == (const publickey & rho) const
    {
        return !(memcmp(this->_bytes, rho._bytes, size));
    }

    // Casting

    box :: publickey :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // Private casting

    box :: publickey :: operator uint8_t * ()
    {
        return this->_bytes;
    }

    // secretkey

    // Operators

    const uint8_t & box :: secretkey :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    bool box :: secretkey :: operator == (const secretkey & rho) const
    {
        return !(memcmp(this->_bytes, rho._bytes, size));
    }

    // Casting

    box :: secretkey :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // Private casting

    box :: secretkey :: operator uint8_t * ()
    {
        return this->_bytes;
    }

    // box

    // Constructors

    box :: box()
    {
        crypto_box_keypair(this->_publickey, this->_secretkey);
    }

    // Getters

    const class box :: publickey & box :: publickey() const
    {
        return this->_publickey;
    }

    const class box :: secretkey & box :: secretkey() const
    {
        return this->_secretkey;
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const class box :: publickey & key)
    {
        out << "<";

        for(size_t i = 0; i < box :: publickey :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(key[i]);

        out << ">";

        return out;
    }

    std :: ostream & operator << (std :: ostream & out, const class box :: secretkey & key)
    {
        out << "<";

        for(size_t i = 0; i < box :: secretkey :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(key[i]);

        out << ">";

        return out;
    }
};
