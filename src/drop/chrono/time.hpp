#ifndef __drop__chrono__time__hpp
#define __drop__chrono__time__hpp

// Includes

#include "time.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // timestamp

    // Methods

    template <typename vtype> void timestamp :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_value);
    }

    template <typename vtype> void timestamp :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_value);
    }

    // interval

    // Constexpr constructors

    constexpr interval :: interval(const uint64_t & value) : _value(value)
    {
    }

    // Methods

    template <typename vtype> void interval :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_value);
    }

    template <typename vtype> void interval :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_value);
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
