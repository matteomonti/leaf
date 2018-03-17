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
#include "drop/data/buffer.h"
#include "drop/data/tag.h"

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

        size_t operator () (const buffer &) const noexcept;
        template <size_t length> size_t operator () (const tag <length> &) const noexcept;

        template <typename type, std :: enable_if_t <bytewise :: constraints :: serializable <type> ()> * = nullptr> size_t operator () (const type &) const noexcept;
    };
};

#endif
