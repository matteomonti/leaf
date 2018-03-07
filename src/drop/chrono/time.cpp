// Includes

#include "time.h"

namespace drop
{
    // timestamp

    // Constructors

    timestamp :: timestamp()
    {
    }

    timestamp :: timestamp(class now)
    {
        struct timeval time;
        gettimeofday(&time, nullptr);
        this->_value = time.tv_sec * 1000000 + time.tv_usec;
    }

    // Operators

    interval timestamp :: operator - (const timestamp & rho) const
    {
        return interval(this->_value - rho._value);
    }

    // Casting

    timestamp :: operator const uint64_t & () const
    {
        return this->_value;
    }

    // interval

    interval :: interval()
    {
    }

    interval :: interval(const uint64_t & value) : _value(value)
    {
    }

    // Casting

    interval :: operator const uint64_t & () const
    {
        return this->_value;
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const timestamp & timestamp)
    {
        time_t seconds = ((const uint64_t &) timestamp) / 1000000;
        tm * timeinfo = localtime(&seconds);

        char buffer[80];
        strftime(buffer, 80, "[%H:%M:%S (%b %d, %Y)]", timeinfo);

        return out << buffer;
    }

    std :: ostream & operator << (std :: ostream & out, const interval & interval)
    {
        uint64_t microseconds = interval;

        if(microseconds < 1000ull)
            out << "(" << microseconds << "us)";
        else if(interval < 1000000ull)
            out << "(" << std :: setprecision(4) << (((double) microseconds) / 1000ull) << "ms)";
        else if(interval < 60000000ull)
            out << "(" << std :: setprecision(4) << (((double) microseconds) / 1000000ull) << "s)";
        else if(interval < 3600000000ull)
            out << "(" << (microseconds / 60000000ull) << "m " << std :: setprecision(4) << (((double) (microseconds % 60000000ull)) / 1000000ull) << "s)";
        else
            out << "(" << (microseconds / 3600000000ull) << "h " << ((microseconds / 60000000ull) % 60) << "m " << std :: setprecision(4) << (((double) (microseconds % 60000000ull)) / 1000000ull) << "s)";
        return out;
    }

    // Literals

    interval operator "" _us(uint64_t value)
    {
        return interval(value);
    }

    interval operator "" _ms(uint64_t value)
    {
        value *= 1000ull;
        return interval(value);
    }

    interval operator "" _ms(long double value)
    {
        value *= 1000ull;
        return interval(value);
    }

    interval operator "" _s(uint64_t value)
    {
        value *= 1000000ull;
        return interval(value);
    }

    interval operator "" _s(long double value)
    {
        value *= 1000000ull;
        return interval(value);
    }

    interval operator "" _m(uint64_t value)
    {
        value *= 60000000ull;
        return interval(value);
    }

    interval operator "" _m(long double value)
    {
        value *= 60000000ull;
        return interval(value);
    }

    interval operator "" _h(uint64_t value)
    {
        value *= 3600000000ull;
        return interval(value);
    }

    interval operator "" _h(long double value)
    {
        value *= 3600000000ull;
        return interval(value);
    }

    // Static declarations

    class now now;
};
