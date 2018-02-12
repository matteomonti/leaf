// Forward declarations

namespace drop
{
    class sha256;
};

#if !defined(__forward__) && !defined(__drop__crypto__sha256__h)
#define __drop__crypto__sha256__h

// Libraries

#include <openssl/sha.h>
#include <openssl/evp.h>

#include <iostream>
#include <iomanip>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    class sha256
    {
    public:

        // Nested classes

        class digest
        {
        public:

            // Properties

            static constexpr size_t size = SHA256_DIGEST_LENGTH;

        private:

            // Members

            uint8_t _bytes[size];

        public:

            // Constructors

            digest() = default;
            digest(SHA256_CTX &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            bool operator == (const digest &) const;

            // Casting

            operator const uint8_t * () const;
        };

    private:

        // Members

        SHA256_CTX _ctx;

    public:

        // Constructors

        sha256();
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, sha256> ()))> * = nullptr> sha256(const atypes & ...);

        // Methods

        void update(const uint8_t *, const size_t &);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, sha256> ()))> * = nullptr> void update(const atypes & ...);

        // Casting

        operator digest ();
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const sha256 :: digest &);
};

#endif
