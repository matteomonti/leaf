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

    // Casting

    timestamp :: operator const uint64_t & () const
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

    // Static declarations

    class now now;
};
