// Forward declarations

namespace drop
{
    class hash;
    class hasher;
};

#if !defined(__forward__) && !defined(__drop__crypto__hash__h)
#define __drop__crypto__hash__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/tag.h"

namespace drop
{
    class hash : public tag <crypto_generichash_BYTES>
    {
        // Friends

        friend class hasher;

    public:

        // Nested classes

        class key : public tag <crypto_generichash_KEYBYTES>
        {
        public:

            // Static methods

            static key random();
        };

        // Constructors

        hash() = default;
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> * = nullptr> hash(const atypes & ...);

    private:

        // Private constructors

        hash(crypto_generichash_state &);

    public:

        // Static methods

        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> * = nullptr> static hash keyed(const key &, const atypes & ...);
    };

    // Ostream integration

    class hasher
    {
        // Members

        crypto_generichash_state _state;

    public:

        // Constructors

        hasher();
        hasher(const hash :: key &);

        // Methods

        hasher & update(const uint8_t *, const size_t &);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> * = nullptr> hasher & update(const atypes & ...);
        hash finalize();
    };
};

#endif
