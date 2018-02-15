// Includes

#include "box.h"

namespace drop
{
    // Exceptions

    const char * box :: exceptions :: encryption_failed :: what() const throw()
    {
        return "Failed to encrypt the message.";
    }

    const char * box :: exceptions :: decryption_failed :: what() const throw()
    {
        return "Failed to decrypt the message (message too short, or MAC not successfully verified).";
    }

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

    box :: box(const class publickey & publickey, const class secretkey & secretkey) : _publickey(publickey), _secretkey(secretkey)
    {
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

    // Methods

    buffer box :: encrypt(const class publickey & to, const buffer & plaintext)
    {
        buffer ciphertext;
        ciphertext.alloc(nonce :: size + mac :: size + plaintext.size());
        randombytes_buf((uint8_t *) ciphertext, nonce :: size);

        if(crypto_box_easy((uint8_t *) ciphertext + nonce :: size, plaintext, plaintext.size(), ciphertext, to, this->_secretkey))
            throw exceptions :: encryption_failed();

        return ciphertext;
    }

    buffer box :: decrypt(const class publickey & from, const buffer & ciphertext)
    {
        if(ciphertext.size() < nonce :: size + mac :: size)
            throw exceptions :: decryption_failed();

        buffer plaintext;
        plaintext.alloc(ciphertext.size() - nonce :: size - mac :: size);

        if(crypto_box_open_easy(plaintext, (const uint8_t *) ciphertext + nonce :: size, ciphertext.size() - nonce :: size, ciphertext, from, this->_secretkey))
            throw exceptions :: decryption_failed();

        return plaintext;
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
