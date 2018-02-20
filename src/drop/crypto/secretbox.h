// Forward declarations

namespace drop
{
    class secretbox;
};

#if !defined(__forward__) && !defined(__drop__crypto__secretbox__h)
#define __drop__crypto__secretbox__h

// Libraries

#include <iostream>
#include <exception>
#include <sodium.h>

// Includes

#include "drop/data/buffer.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/tag.h"

namespace drop
{
    class secretbox
    {
    public:

        // Exceptions

        struct exceptions
        {
            class decryption_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Nested classes

        class key : public tag <crypto_secretbox_KEYBYTES>
        {
        public:

            // Static methods

            static key random();
        };

        class nonce : public tag <crypto_secretbox_NONCEBYTES>
        {
        public:

            // Operators

            nonce & operator ++ ();
            nonce & operator ++ (int);

            // Static methods

            static nonce random();
        };

        class mac
        {
        public:

            // Static members

            static constexpr size_t size = crypto_secretbox_MACBYTES;
        };

    private:

        // Members

        key _key;
        nonce _nonce;

    public:

        // Constructors

        secretbox();
        secretbox(const key &);
        secretbox(const key &, const nonce &);
        secretbox(const secretbox &) = delete;

        // Getters

        const key & key() const;
        const nonce & nonce() const;

        // Methods

        buffer encrypt(const buffer &);
        buffer decrypt(const buffer &);

        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> * = nullptr> buffer encrypt(const atypes & ...);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <atypes> ()))> * = nullptr> auto decrypt(const buffer &);

        // Operators

        secretbox & operator = (const secretbox &) = delete;
    };
};

#endif
