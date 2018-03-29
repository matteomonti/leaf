// Includes

#include "local.hpp"

namespace drop :: sockets
{
    // Private constructors

    local :: local(const int & descriptor) : _descriptor(descriptor), _blocking(true)
    {
    }

    // Getters

    const int & local :: descriptor() const
    {
        return this->_descriptor;
    }

    const address & local :: remote() const
    {
        static address remote("0.0.0.0", 0);
        return remote;
    }

    // Setters

    void local :: send_timeout(const interval & timeout)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        struct timeval timeval;

        timeval.tv_sec = ((const uint64_t &) timeout) / 1000000;
        timeval.tv_usec = ((const uint64_t &) timeout) % 1000000;

        if(:: setsockopt(this->_descriptor, SOL_SOCKET, SO_SNDTIMEO, (uint8_t *)&timeval, sizeof(struct timeval)))
            throw exceptions :: setsockopt_failed();
    }

    void local :: receive_timeout(const interval & timeout)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        struct timeval timeval;

        timeval.tv_sec = ((const uint64_t &) timeout) / 1000000;
        timeval.tv_usec = ((const uint64_t &) timeout) % 1000000;

        if(:: setsockopt(this->_descriptor, SOL_SOCKET, SO_RCVTIMEO, (uint8_t *)&timeval, sizeof(struct timeval)))
            throw exceptions :: setsockopt_failed();
    }

    void local :: block(const bool & value)
    {
        int flags = fcntl(this->_descriptor, F_GETFL, 0);

        if(!value)
            fcntl(this->_descriptor, F_SETFL, flags | O_NONBLOCK);
        else
            fcntl(this->_descriptor, F_SETFL, flags & (!O_NONBLOCK));

        this->_blocking = value;
    }

    // Methods

    size_t local :: available()
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        int value;

        if(ioctl(this->_descriptor, FIONREAD, &value))
            throw exceptions :: ioctl_failed();

        return value;
    }

    size_t local :: send(const uint8_t * message, const size_t & size)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        ssize_t res = :: send(this->_descriptor, message, size, 0);

        if(res < 0)
        {
            if(!(this->_blocking) && errno == EWOULDBLOCK)
                return 0;
            else if(errno == EAGAIN)
                throw exceptions :: send_timeout();
            else
                throw exceptions :: send_failed();
        }

        return (size_t) res;
    }

    size_t local :: receive(uint8_t * message, const size_t & size)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        ssize_t res = :: recv(this->_descriptor, message, size, 0);

        if(res < 0)
        {
            if(!(this->_blocking) && errno == EWOULDBLOCK)
                return 0;
            else if(errno == EAGAIN)
                throw exceptions :: receive_timeout();
            else
                throw exceptions :: receive_failed();
        }
        else if(res == 0)
            throw exceptions :: connection_closed();

        return (size_t) res;
    }

    void local :: close()
    {
        :: close(this->_descriptor);
        this->_descriptor = -1;
    }
};
