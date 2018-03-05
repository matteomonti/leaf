// Forward declarations libraries

#include <stddef.h>

// Forward declarations

namespace drop
{
    template <size_t> class tag;
};

#if !defined(__forward__) && !defined(__drop__data__tag__h)
#define __drop__data__tag__h

// Libraries

#include <iostream>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    template <size_t length> class tag
    {
    public:

        // Properties

        static constexpr size_t size = length;

    protected:

        // Members

        uint8_t _bytes[size];

    public:

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        const uint8_t & operator [] (const size_t &) const;

        bool operator == (const tag &) const;
        bool operator != (const tag &) const;

        bool operator < (const tag &) const;
        bool operator <= (const tag &) const;
        bool operator > (const tag &) const;
        bool operator >= (const tag &) const;

        // Casting

        operator const uint8_t * () const;

    protected:

        // Protected casting

        explicit operator uint8_t * ();
    };

    // Ostream integration

    template <size_t length> std :: ostream & operator << (std :: ostream &, const tag <length> &);
};

#endif
