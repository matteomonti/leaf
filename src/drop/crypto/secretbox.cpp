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

    // Static methods

    class secretbox :: key secretbox :: key :: random()
    {
        key key;
        randombytes_buf(key._bytes, size);
        return key;
    }

    // nonce

    // Operators

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
};
