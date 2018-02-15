// Includes

#include "secretbox.hpp"

namespace drop
{
    // Exceptions

    const char * secretbox :: exceptions :: decryption_failed :: what() const throw()
    {
        return "Failed to decrypt the message (message too short, or MAC not successfully verified).";
    }

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

    class secretbox :: key secretbox :: key :: random()
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

    class secretbox :: nonce & secretbox :: nonce :: operator ++ ()
    {
        sodium_increment(this->_bytes, size);
        return (*this);
    }

    class secretbox :: nonce & secretbox :: nonce :: operator ++ (int)
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

    class secretbox :: nonce secretbox :: nonce :: random()
    {
        nonce nonce;
        randombytes_buf(nonce._bytes, size);
        return nonce;
    }

    // secretbox

    // Constructors

    secretbox :: secretbox() : _key(key :: random()), _nonce(nonce :: random())
    {
    }

    secretbox :: secretbox(const class key & key) : _key(key), _nonce(nonce :: random())
    {
    }

    secretbox :: secretbox(const class key & key, const class nonce & nonce) : _key(key), _nonce(nonce)
    {
    }

    // Getters

    const class secretbox :: key & secretbox :: key() const
    {
        return this->_key;
    }

    const class secretbox :: nonce & secretbox :: nonce() const
    {
        return this->_nonce;
    }

    // Methods

    buffer secretbox :: encrypt(const buffer & plaintext)
    {
        (this->_nonce)++;

        buffer ciphertext;
        ciphertext.alloc(mac :: size + plaintext.size());

        crypto_secretbox_easy(ciphertext, plaintext, plaintext.size(), this->_nonce, this->_key);

        return ciphertext;
    }

    buffer secretbox :: decrypt(const buffer & ciphertext)
    {
        (this->_nonce)++;

        if(ciphertext.size() < mac :: size)
            throw exceptions :: decryption_failed();

        buffer plaintext;
        plaintext.alloc(ciphertext.size() - mac :: size);

        if(crypto_secretbox_open_easy(plaintext, ciphertext, ciphertext.size(), this->_nonce, this->_key))
            throw exceptions :: decryption_failed();

        return plaintext;
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const class secretbox :: key & key)
    {
        out << "<";

        for(size_t i = 0; i < secretbox :: key :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(key[i]);

        out << ">";

        return out;
    }

    std :: ostream & operator << (std :: ostream & out, const class secretbox :: nonce & nonce)
    {
        out << "<";

        for(size_t i = 0; i < secretbox :: nonce :: size; i++)
            out << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int)(nonce[i]);

        out << ">";

        return out;
    }
};
