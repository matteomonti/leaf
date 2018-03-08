// Includes

#include "udp.h"

namespace drop :: sockets
{
    // Settings

    constexpr size_t udp :: settings :: mtu;

    // packet

    // Getters

    const address & udp :: packet :: remote() const
    {
        return this->_remote;
    }

    const uint8_t * udp :: packet :: message() const
    {
        return this->_message;
    }

    const size_t & udp :: packet :: size() const
    {
        return this->_size;
    }

    // Casting

    udp :: packet :: operator bool () const
    {
        return (bool) (this->_size);
    }

    // udp

    // Constructors

    udp :: udp() : _blocking(true)
    {
        this->_descriptor = :: socket(PF_INET, SOCK_DGRAM, 0);

        int optreuse = 1;
        if (:: setsockopt(this->_descriptor, SOL_SOCKET, SO_REUSEADDR, &optreuse, sizeof(int)))
            throw exceptions :: setsockopt_failed();
    }

    // Getters

    const int & udp :: descriptor() const
    {
        return this->_descriptor;
    }

    // Setters

    void udp :: send_timeout(const interval & timeout)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        struct timeval timeval;

        timeval.tv_sec = ((const uint64_t &) timeout) / 1000000;
        timeval.tv_usec = ((const uint64_t &) timeout) % 1000000;

        if(:: setsockopt(this->_descriptor, SOL_SOCKET, SO_SNDTIMEO, (uint8_t *)&timeval, sizeof(struct timeval)))
            throw exceptions :: setsockopt_failed();
    }

    void udp :: receive_timeout(const interval & timeout)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        struct timeval timeval;

        timeval.tv_sec = ((const uint64_t &) timeout) / 1000000;
        timeval.tv_usec = ((const uint64_t &) timeout) % 1000000;

        if(:: setsockopt(this->_descriptor, SOL_SOCKET, SO_RCVTIMEO, (uint8_t *)&timeval, sizeof(struct timeval)))
            throw exceptions :: setsockopt_failed();
    }

    void udp :: block(const bool & value)
    {
        int flags = fcntl(this->_descriptor, F_GETFL, 0);

        if(!value)
            fcntl(this->_descriptor, F_SETFL, flags | O_NONBLOCK);
        else
            fcntl(this->_descriptor, F_SETFL, flags & (!O_NONBLOCK));

        this->_blocking = value;
    }

    // Methods

    void udp :: bind(const class address :: port & port)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        address address(any, port);

        if(:: bind(this->_descriptor, (const struct sockaddr *) &(const sockaddr_in &)(address), sizeof(sockaddr_in)))
            throw exceptions :: bind_failed();

        this->_port = port;
    }

    bool udp :: send(const address & remote, const uint8_t * message, const size_t & size)
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        if(:: sendto(this->_descriptor, message, size, 0, (struct sockaddr *) &remote, sizeof(sockaddr_in)) < 0)
        {
            if(!(this->_blocking) && errno == EWOULDBLOCK)
                return false;
            else if(errno == EAGAIN)
                throw exceptions :: send_timeout();
            else
                throw exceptions :: send_failed();
        }

        return true;
    }

    udp :: packet udp :: receive()
    {
        if(this->_descriptor < 0)
            throw exceptions :: socket_closed();

        packet packet;
        socklen_t remote_len = sizeof(sockaddr_in);

        int status;
        if((status = :: recvfrom(this->_descriptor, packet._message, settings :: mtu, 0, (sockaddr *) &(packet._remote), &remote_len)) < 0)
        {
            if(!(this->_blocking) && errno == EWOULDBLOCK)
                status = 0;
            else if(errno == EAGAIN)
                throw exceptions :: receive_timeout();
            else
                throw exceptions :: receive_failed();
        }

        packet._size = status;
        packet._message[packet._size] = '\0';
        return packet;
    }

    void udp :: close()
    {
        :: close(this->_descriptor);
        this->_descriptor = -1;
    }
};
