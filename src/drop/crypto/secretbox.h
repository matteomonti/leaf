// Forward declarations

namespace drop
{
    class secretbox;
};

#if !defined(__forward__) && !defined(__drop__crypto__secretbox__h)
#define __drop__crypto__secretbox__h

// Libraries

#include <iostream>
#include <sodium.h>

// Includes

#include "drop/data/buffer.h"
#include "drop/bytewise/bytewise.hpp"

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

        class key
        {
        public:

            // Properties

            static constexpr size_t size = crypto_secretbox_KEYBYTES;

        private:

            // Members

            uint8_t _bytes[size];

        public:

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            bool operator == (const key &) const;

            // Casting

            operator const uint8_t * () const;

            // Static methods

            static key random();
        };

        class nonce
        {
        public:

            // Properties

            static constexpr size_t size = crypto_secretbox_NONCEBYTES;

        private:

            // Members

            uint8_t _bytes[size];

        public:

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            nonce & operator ++ ();
            nonce & operator ++ (int);

            bool operator == (const nonce &) const;

            // Casting

            operator const uint8_t * () const;

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

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const class secretbox :: key &);
    std :: ostream & operator << (std :: ostream &, const class secretbox :: nonce &);
};

#endif
