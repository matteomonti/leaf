#ifndef __drop__data__tag__hpp
#define __drop__data__tag__hpp

// Includes

#include "tag.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // Methods

    template <size_t length> template <typename vtype> void tag <length> :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <size_t length> template <typename vtype> void tag <length> :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // Operators

    template <size_t length> const uint8_t & tag <length> :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    template <size_t length> bool tag <length> :: operator == (const tag <length> & rho) const
    {
        return !(memcmp(this->_bytes, rho._bytes, size));
    }

    template <size_t length> bool tag <length> :: operator != (const tag <length> & rho) const
    {
        return !((*this) == rho);
    }

    template <size_t length> bool tag <length> :: operator < (const tag <length> & rho) const
    {
        return (memcmp(this->_bytes, rho._bytes, size) < 0);
    }

    template <size_t length> bool tag <length> :: operator <= (const tag <length> & rho) const
    {
        return (memcmp(this->_bytes, rho._bytes, size) <= 0);
    }

    template <size_t length> bool tag <length> :: operator > (const tag <length> & rho) const
    {
        return (memcmp(this->_bytes, rho._bytes, size) > 0);
    }

    template <size_t length> bool tag <length> :: operator >= (const tag <length> & rho) const
    {
        return (memcmp(this->_bytes, rho._bytes, size) >= 0);
    }

    // Casting

    template <size_t length> tag <length> :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // Protected casting

    template <size_t length> tag <length> :: operator uint8_t * ()
    {
        return this->_bytes;
    }

    // Ostream integration

    template <size_t length> std :: ostream & operator << (std :: ostream & out, const tag <length> & tag)
    {
        out << std :: hex << "<";

        for(size_t i = 0; i < length; i++)
            out << std :: setw(2) << std :: setfill('0') << (unsigned int)(tag[i]);

        out << ">" << std :: dec;

        return out;
    }
};

#endif
