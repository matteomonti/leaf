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

namespace drop
{
    class secretbox
    {
    public:
        
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
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const secretbox :: key &);
};

#endif
