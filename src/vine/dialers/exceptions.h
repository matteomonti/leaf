// Forward declarations

namespace vine
{
    namespace dialers
    {
        struct exceptions;
    };
};

#if !defined(__forward__) && !defined(__vine__dialers__exceptions__h)
#define __vine__dialers__exceptions__h

// Libraries

#include <exception>

namespace vine :: dialers
{
    struct exceptions
    {
        class node_not_found : public std :: exception
        {
            const char * what() const throw();
        };
    };
};

#endif
