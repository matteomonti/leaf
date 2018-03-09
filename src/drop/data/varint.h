// Forward declarations

namespace drops
{
    class varint;
};

#if !defined(__forward__) && !defined(__drop__data__varint__h)
#define __drop__data__varint__h

// Libraries

#include <stdint.h>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    class varint
    {
    public:

        // Nested classes

        class streamer;

    private:

        // Members

        uint32_t _value;

    public:

        // Constructors

        varint() = default;
        varint(const uint32_t &);

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        varint & operator = (const uint32_t &);

        // Casting

        operator uint32_t & ();
        operator const uint32_t & () const;
    };

    class varint :: streamer
    {
        // Members

        uint8_t _bytes[sizeof(uint32_t)];
        uint8_t _cursor;

    public:

        // Constructors

        streamer();

        // Methods

        void update(const uint8_t *, const size_t &);
        size_t pending();

        varint yield();
    };
};

#endif
