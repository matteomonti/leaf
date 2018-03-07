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

        // Casting

        operator const uint64_t & () const;
    };

    class now
    {
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const timestamp &);

    extern now now;
};

#endif
