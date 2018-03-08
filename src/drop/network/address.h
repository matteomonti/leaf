// Forward declarations

namespace drop
{
    class address;
    class any;
};

#if !defined(__forward__) && !defined(__drop__network__address__h)
#define __drop__network__address__h

// Libraries

#include <stddef.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <iostream>

// Includes

#include "drop/bytewise/bytewise.h"

namespace drop
{
    class address
    {
    public:

        // Exceptions

        struct exceptions
        {
            class host_not_found : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Nested classes

        class ip
        {
            // Members

            in_addr _ip;

        public:

            // Constructors

            ip();
            ip(any);
            ip(const char *);
            ip(const address &);

            // Operators

            bool operator == (const ip &) const;

            // Casting

            operator bool () const;

            explicit operator in_addr & ();
            explicit operator const in_addr & () const;
        };
    };

    class any
    {
    };

    extern any any;
};

#endif
