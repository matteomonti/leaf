// Includes

#include "signature.hpp"

namespace drop
{
    // Exceptions

    const char * signer :: exceptions :: signature_failed :: what() const throw()
    {
        return "Signature failed.";
    }

    // signer

    // Constructors

    signer :: signer()
    {
        crypto_sign_keypair((uint8_t *)(this->_publickey), (uint8_t *)(this->_secretkey));
    }

    signer :: signer(const signature :: publickey & publickey, const signature :: secretkey & secretkey) : _publickey(publickey), _secretkey(secretkey)
    {
    }

    // Getters

    const signature :: publickey & signer :: publickey() const
    {
        return this->_publickey;
    }

    const signature :: secretkey & signer :: secretkey() const
    {
        return this->_secretkey;
    }

    // Methods

    signature signer :: sign(const buffer & message)
    {
        signature signature;

        if(crypto_sign_detached((uint8_t *) signature, nullptr, message, message.size(), this->_secretkey))
            throw exceptions :: signature_failed();

        return signature;
    }

    // Exceptions

    const char * verifier :: exceptions :: verification_failed :: what() const throw()
    {
        return "Verification failed.";
    }

    // verifier

    // Constructors

    verifier :: verifier(const signature :: publickey & publickey) : _publickey(publickey)
    {
    }

    // Getters

    const signature :: publickey & verifier :: publickey() const
    {
        return this->_publickey;
    }

    // Methods

    void verifier :: verify(const signature & signature, const buffer & message)
    {
        if(crypto_sign_verify_detached(signature, message, message.size(), this->_publickey))
            throw exceptions :: verification_failed();
    }
};
