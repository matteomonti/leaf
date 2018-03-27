// Forward declarations

namespace drop
{
    namespace sockets
    {
        class sigmasker;
    };
};

#if !defined(__forward__) && !defined(__drop__network__sockets__sigmasker__h)
#define __drop__network__sockets__sigmasker__h

// Libraries

#include <signal.h>
#include <iostream>

namespace drop :: sockets
{
    class sigmasker
    {
        // Private constructors

        sigmasker();

        // Private static members

        static sigmasker masker;
    };
};

#endif
