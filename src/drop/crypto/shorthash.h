// Forward declarations

namespace drop
{
    class shorthash;
};

#if !defined(__forward__) && !defined(__drop__crypto__shorthash__h)
#define __drop__crypto__shorthash__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    class shorthash
    {
        // Members

        uint8_t _key[crypto_shorthash_KEYBYTES];

    public:

        // Constructors

        shorthash();

        // Operators

        template <typename type, std :: enable_if_t <bytewise :: constraints :: serializable <type> ()> * = nullptr> size_t operator () (const type &) const noexcept;
    };
};

#endif
