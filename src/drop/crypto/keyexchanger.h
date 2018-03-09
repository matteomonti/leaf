// Forward declarations

namespace drop
{
    class keyexchanger;
}

#if !defined(__forward__) && !defined(__drop__crypto__keyexchanger__h)
#define __drop__crypto__keyexchanger__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/data/tag.hpp"
#include "secretbox.hpp"
#include "drop/bytewise/endianess.h"

namespace drop
{
    class keyexchanger
    {
    public:

        // Exceptions

        struct exceptions
        {
            class exchange_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Nested classes

        class publickey : public tag <crypto_kx_PUBLICKEYBYTES>
        {
            // Friends

            friend class keyexchanger;

            // Private operators

            bool operator < (const publickey &) const;
        };

        class secretkey : public tag <crypto_kx_SECRETKEYBYTES>
        {
            // Friends

            friend class keyexchanger;
        };

        class sessionkey : public tag <crypto_kx_SESSIONKEYBYTES>
        {
            // Asserts

            static_assert(crypto_kx_SESSIONKEYBYTES == crypto_kdf_KEYBYTES, "Key exchange session key size and key derivation function master key size differ. Padding or cropping needed.");

            // Friends

            friend class keyexchanger;

            // Members

            bool _lesser;

        public:

            // Methods

            class secretbox :: key transmit();
            class secretbox :: key receive();

            uint64_t number(const char *, const uint64_t &);
        };

    private:

        publickey _publickey;
        secretkey _secretkey;

    public:

        // Constructors

        keyexchanger();
        keyexchanger(const publickey &, const secretkey &);

        // Getters

        const publickey & publickey() const;
        const secretkey & secretkey() const;

        // Methods

        sessionkey exchange(const class publickey &) const;
    };
};

#endif
