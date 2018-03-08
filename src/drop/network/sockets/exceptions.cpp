// Includes

#include "exceptions.h"

namespace drop :: sockets
{
    // Exceptions

    const char * exceptions :: getsockopt_failed :: what() const noexcept
    {
        return "Getsockopt failed.";
    }

    const char * exceptions :: setsockopt_failed :: what() const noexcept
    {
        return "Setsockopt failed.";
    }

    const char * exceptions :: socket_closed :: what() const noexcept
    {
        return "Socket closed.";
    }

    const char * exceptions :: bind_failed :: what() const noexcept
    {
        return "Bind failed.";
    }

    const char * exceptions :: connect_failed :: what() const noexcept
    {
        return "Connect failed.";
    }

    const char * exceptions :: listen_failed :: what() const noexcept
    {
        return "Listen failed.";
    }

    const char * exceptions :: accept_failed :: what() const noexcept
    {
        return "Accept failed.";
    }

    const char * exceptions :: send_timeout :: what() const noexcept
    {
        return "Send timeout.";
    }

    const char * exceptions :: send_failed :: what() const noexcept
    {
        return "Send failed.";
    }

    const char * exceptions :: receive_timeout :: what() const noexcept
    {
        return "Receive timeout.";
    }

    const char * exceptions :: receive_failed :: what() const noexcept
    {
        return "Receive failed.";
    }

    const char * exceptions :: connect_timeout :: what() const noexcept
    {
        return "Connect timeout.";
    }

    const char * exceptions :: connection_closed :: what() const noexcept
    {
        return "Connection closed by remote endpoint.";
    }
};
