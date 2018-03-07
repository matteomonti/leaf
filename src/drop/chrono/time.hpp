#ifndef __drop__chrono__time__hpp
#define __drop__chrono__time__hpp

// Includes

#include "time.h"

namespace drop
{
    // interval

    // Constexpr constructors

    constexpr interval :: interval(const uint64_t & value) : _value(value)
    {
    }

    // Literals

    constexpr interval operator "" _us(uint64_t value)
    {
        return interval(value);
    }

    constexpr interval operator "" _ms(uint64_t value)
    {
        value *= 1000ull;
        return interval(value);
    }

    constexpr interval operator "" _ms(long double value)
    {
        value *= 1000ull;
        return interval(value);
    }

    constexpr interval operator "" _s(uint64_t value)
    {
        value *= 1000000ull;
        return interval(value);
    }

    constexpr interval operator "" _s(long double value)
    {
        value *= 1000000ull;
        return interval(value);
    }

    constexpr interval operator "" _m(uint64_t value)
    {
        value *= 60000000ull;
        return interval(value);
    }

    constexpr interval operator "" _m(long double value)
    {
        value *= 60000000ull;
        return interval(value);
    }

    constexpr interval operator "" _h(uint64_t value)
    {
        value *= 3600000000ull;
        return interval(value);
    }

    constexpr interval operator "" _h(long double value)
    {
        value *= 3600000000ull;
        return interval(value);
    }
};

#endif
