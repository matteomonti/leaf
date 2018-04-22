// Forward declarations

namespace drop
{
    class buffer;
};

#if !defined(__forward__) && !defined(__drop__data__buffer__h)
#define __drop__data__buffer__h

// Libraries

#include <stddef.h>
#include <string.h>
#include <utility>
#include <iostream>
#include <iomanip>

// Includes

#include "varint.h"
#include "drop/bytewise/bytewise.h"

namespace drop
{
    class buffer
    {
    public:

        // Nested classes

        class streamer;

        class reference
        {
        };

    private:

        // Members

        uint8_t * _bytes;
        size_t _size;
        size_t _alloc;

    public:

        // Constructors

        buffer();
        buffer(const uint8_t *, const size_t &);
        buffer(const char *, const size_t &);
        buffer(const char *);

        buffer(class reference, uint8_t *, const size_t &);

        buffer(const buffer &);
        buffer(buffer &&);

        // Destructor

        ~buffer();

        // Getters

        const size_t & size() const;

        // Methods

        void alloc(const size_t &);

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        uint8_t & operator [] (const size_t &);
        const uint8_t & operator [] (const size_t &) const;

        bool operator == (const buffer &) const;
        bool operator != (const buffer &) const;

        buffer & operator = (const buffer &);
        buffer & operator = (buffer &&);

        // Casting

        operator uint8_t * ();
        operator const uint8_t * () const;

        operator char * ();
        operator const char * () const;

        // Static members

        static reference reference;
    };

    class buffer :: streamer
    {
        // Members

        buffer _buffer;
        size_t _cursor;

    public:

        // Constructors

        streamer(const size_t &);

        // Methods

        void update(const uint8_t *, const size_t &);
        size_t pending();

        buffer && yield();
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const buffer &);
};

#endif
