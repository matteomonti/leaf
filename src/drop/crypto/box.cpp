// Includes

#include "box.hpp"

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

    // box

    // Constructors

    box :: box()
    {
        crypto_box_keypair((uint8_t *) this->_publickey, (uint8_t *) this->_secretkey);
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
};
