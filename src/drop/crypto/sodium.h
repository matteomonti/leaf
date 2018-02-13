// Forward declarations

namespace drop
{
    class sodium;
};

#if !defined(__forward__) && !defined(__drop__crypto__sodium__h)
#define __drop__crypto__sodium__h

// Libraries

#include <sodium.h>
#include <exception>

namespace drop
{
    class sodium
    {
    public:

        // Exceptions

        struct exceptions
        {
            class init_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

    private:

        // Private constructors

        sodium();

        // Static members

        static sodium singleton;
    };
};

#endif
