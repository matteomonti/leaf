// Includes

#include "sha256.hpp"

namespace drop
{
    // digest

    // Constructors

    sha256 :: digest :: digest(SHA256_CTX & ctx)
    {
        SHA256_Final(this->_bytes, &ctx);
    }

    // Operatorssha2

    const uint8_t & sha256 :: digest :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    // Casting

    sha256 :: digest :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    // sha256

    // Constructors

    sha256 :: sha256()
    {
        SHA256_Init(&(this->_ctx));
    }

    // Methods

    void sha256 :: update(const uint8_t * bytes, const size_t & size)
    {
        SHA256_Update(&(this->_ctx), bytes, size);
    }

    // Casting

    sha256 :: operator digest()
    {
        return digest(this->_ctx);
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const sha256 :: digest & digest)
    {
        out << "<";

        for(size_t i = 0; i < sha256 :: digest :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(digest[i]);

        out << ">";
        
        return out;
    }
};
