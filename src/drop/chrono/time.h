// Forward declarations

namespace drop
{
    class timestamp;
    class interval;
    class now;
};

#if !defined(__forward__) && !defined(__drop__chrono__time__h)
#define __drop__chrono__time__h

// Libraries

#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <iomanip>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    class timestamp
    {
        // Members

        uint64_t _value;

    public:

        // Constructors

        timestamp();
        timestamp(now);

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        timestamp operator + (const interval &) const;
        timestamp operator - (const interval &) const;

        interval operator - (const timestamp &) const;

        bool operator < (const timestamp &) const;
        bool operator <= (const timestamp &) const;

        bool operator > (const timestamp &) const;
        bool operator >= (const timestamp &) const;

        bool operator == (const timestamp &) const;
        bool operator != (const timestamp &) const;

        // Casting

        operator const uint64_t & () const;
    };

    class interval
    {
        // Members

        uint64_t _value;

    public:

        // Constructors

        interval();

        // Constexpr constructors

        constexpr interval(const uint64_t &);

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Casting

        operator const uint64_t & () const;
    };

    // Literals

    constexpr interval operator "" _us(uint64_t);

    constexpr interval operator "" _ms(uint64_t);
    constexpr interval operator "" _ms(long double);

    constexpr interval operator "" _s(uint64_t);
    constexpr interval operator "" _s(long double);

    constexpr interval operator "" _m(uint64_t);
    constexpr interval operator "" _m(long double);

    constexpr interval operator "" _h(uint64_t);
    constexpr interval operator "" _h(long double);

    class now
    {
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const timestamp &);
    std :: ostream & operator << (std :: ostream &, const interval &);

    extern now now;
};

#endif
