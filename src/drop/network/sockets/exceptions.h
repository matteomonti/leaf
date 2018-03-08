// Forward declarations

namespace drop
{
    namespace sockets
    {
        struct exceptions;
    };
};

#if !defined(__forward__) && !defined(__network__sockets__exceptions__h)
#define __network__sockets__exceptions__h

// Libraries

#include <exception>

namespace drop :: sockets
{
    struct exceptions
    {
        class getsockopt_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class setsockopt_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class socket_closed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class bind_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class connect_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class listen_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class accept_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class send_timeout : public std :: exception
        {
            const char * what() const noexcept;
        };

        class send_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class receive_timeout : public std :: exception
        {
            const char * what() const noexcept;
        };

        class receive_failed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class connect_timeout : public std :: exception
        {
            const char * what() const noexcept;
        };

        class connection_closed : public std :: exception
        {
            const char * what() const noexcept;
        };

        class ioctl_failed : public std :: exception
        {
            const char * what() const noexcept;
        };
    };
}

#endif
