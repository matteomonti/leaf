// Forward declarations

namespace drop
{
    class timestamp;
    class interval;
    class frequency;
    class now;
};

#if !defined(__forward__) && !defined(__drop__chrono__time__h)
#define __drop__chrono__time__h

// Libraries

#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <iomanip>

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

        // Operators

        interval operator - (const timestamp &) const;

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
        interval(const uint64_t &);

        // Casting

        operator const uint64_t & () const;
    };

    class now
    {
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const timestamp &);
    std :: ostream & operator << (std :: ostream &, const interval &);

    extern now now;
};

#endif
