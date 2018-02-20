// Includes

#include "keyexchanger.h"

namespace drop
{
    // Exceptions

    const char * keyexchanger :: exceptions :: exchange_failed :: what() const throw()
    {
        return "Key exchange failed.";
    }

    // publickey

    // Private operators

    bool keyexchanger :: publickey :: operator < (const publickey & rho) const
    {
        return (memcmp(this->_bytes, rho._bytes, size) < 0);
    }

    // sessionkey

    // Methods

    class secretbox :: key keyexchanger :: sessionkey :: transmit()
    {
        class secretbox :: key key;
        crypto_kdf_derive_from_key((uint8_t *) key, secretbox :: key :: size, (this->_lesser ? 0 : 1), "rxtx", this->_bytes);
        return key;
    }

    class secretbox :: key keyexchanger :: sessionkey :: receive()
    {
        class secretbox :: key key;
        crypto_kdf_derive_from_key((uint8_t *) key, secretbox :: key :: size, (this->_lesser ? 1 : 0), "rxtx", this->_bytes);
        return key;
    }

    // keyexchanger

    // Constructors

    keyexchanger :: keyexchanger()
    {
        crypto_kx_keypair((uint8_t *)(this->_publickey), (uint8_t *)(this->_secretkey));
    }

    keyexchanger :: keyexchanger(const class publickey & publickey, const class secretkey & secretkey) : _publickey(publickey), _secretkey(secretkey)
    {
    }

    // Getters

    const class keyexchanger :: publickey & keyexchanger :: publickey() const
    {
        return this->_publickey;
    }

    const class keyexchanger :: secretkey & keyexchanger :: secretkey() const
    {
        return this->_secretkey;
    }

    // Methods

    keyexchanger :: sessionkey keyexchanger :: exchange(const class publickey & remote)
    {
        sessionkey sessionkey;

        if(this->_publickey < remote)
        {
            sessionkey._lesser = true;
            if(crypto_kx_server_session_keys((uint8_t *) sessionkey, nullptr, this->_publickey, this->_secretkey, remote))
                throw exceptions :: exchange_failed();
        }
        else
        {
            sessionkey._lesser = false;
            if(crypto_kx_client_session_keys(nullptr, (uint8_t *) sessionkey, this->_publickey, this->_secretkey, remote))
                throw exceptions :: exchange_failed();
        }

        return sessionkey;
    }
};
