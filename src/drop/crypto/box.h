// Forward declarations

namespace drop
{
    class box;
};

#if !defined(__forward__) && !defined(__drop__crypto__box__h)
#define __drop__crypto__box__h

// Libraries

#include <iostream>
#include <exception>
#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/tag.h"

namespace drop
{
    class box
    {
    public:

        // Exceptions

        struct exceptions
        {
            class encryption_failed : public std :: exception
            {
                const char * what() const throw();
            };

            class decryption_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Nested classes

        class publickey : public tag <crypto_box_PUBLICKEYBYTES>
        {
            // Friends

            friend class box;
        };

        class secretkey : public tag <crypto_box_SECRETKEYBYTES>
        {
            // Friends

            friend class box;
        };

        class nonce
        {
        public:

            // Properties

            static constexpr size_t size = crypto_box_NONCEBYTES;
        };

        class mac
        {
        public:

            // Properties

            static constexpr size_t size = crypto_box_MACBYTES;
        };

    private:

        // Members

        publickey _publickey;
        secretkey _secretkey;

    public:

        // Constructors

        box();
        box(const publickey &, const secretkey &);

        // Getters

        const publickey & publickey() const;
        const secretkey & secretkey() const;

        // Methods

        buffer encrypt(const class publickey &, const buffer &);
        buffer decrypt(const class publickey &, const buffer &);

        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> * = nullptr> buffer encrypt(const class publickey &, const atypes & ...);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <atypes> ()))> * = nullptr> auto decrypt(const class publickey &, const buffer &);
    };
};

#endif
