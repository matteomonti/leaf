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

            in_addr _value;

        public:

            // Constructors

            ip();
            ip(any);
            ip(const char *);
            ip(const address &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            bool operator == (const ip &) const;

            // Casting

            operator bool () const;

            explicit operator in_addr & ();
            explicit operator const in_addr & () const;
        };

        class port
        {
            // Members

            in_port_t _value;

        public:

            // Constructors

            port();
            port(const uint16_t &);
            port(const address &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            bool operator == (const port &) const;

            // Casting

            operator bool () const;

            explicit operator in_port_t & ();
            explicit operator const in_port_t & () const;
        };

    private:

        // Members

        sockaddr_in _value;

    public:

        // Constructors

        address();
        address(const char *, const uint16_t &);
        address(const class ip &, const class port &);
        address(const sockaddr_in &);

        // Getters

        class ip ip() const;
        class port port() const;

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        bool operator == (const address &) const;
        bool operator == (const sockaddr_in &) const;

        // Casting

        operator bool () const;

        operator sockaddr_in & ();
        operator const sockaddr_in & () const;
    };

    class any
    {
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const class address :: ip &);
    std :: ostream & operator << (std :: ostream &, const class address :: port &);
    std :: ostream & operator << (std :: ostream &, const address &);

    extern any any;
};

#endif
