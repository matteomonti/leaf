// Forward declarations

namespace drop
{
    class wakepipe;
};

#if !defined(__forward__) && !defined(__drop__thread__wakepipe__h)
#define __drop__thread__wakepipe__h

// Libraries

#include <unistd.h>
#include <fcntl.h>

namespace drop
{
    class wakepipe
    {
        // Members

        int _read;
        int _write;

    public:

        // Constructors

        wakepipe();

        // Methods

        void wake();
        void flush();

        // Casting

        operator const int & () const;
    };
};

#endif
