// Includes

#include "address.hpp"

namespace drop
{
    // Exceptions

    const char * address :: exceptions :: host_not_found :: what() const throw()
    {
        return "Host not found.";
    }

    // ip

    // Constructors

    address :: ip :: ip()
    {
    }

    address :: ip :: ip(class any)
    {
        this->_value.s_addr = htonl(INADDR_ANY);
    }

    address :: ip :: ip(const char * host)
    {
        hostent * record = gethostbyname(host);

        if(record)
            memcpy(&(this->_value), record->h_addr, sizeof(in_addr));
        else
            throw exceptions :: host_not_found();
    }

    address :: ip :: ip(const address & address)
    {
        this->_value.s_addr = address._value.sin_addr.s_addr;
    }

    bool address :: ip :: operator == (const ip & rho) const
    {
        return this->_value.s_addr == rho._value.s_addr;
    }

    // Casting

    address :: ip :: operator bool () const
    {
        return this->_value.s_addr;
    }

    address :: ip :: operator in_addr & ()
    {
        return this->_value;
    }

    address :: ip :: operator const in_addr & () const
    {
        return this->_value;
    }

    // port

    address :: port :: port() : _value()
    {
    }

    address :: port :: port(const uint16_t & port) : _value(htons(port))
    {
    }

    address :: port :: port(const address & address) : _value(address._value.sin_port)
    {
    }

    // Operators

    bool address :: port :: operator == (const port & rho) const
    {
        return this->_value == rho._value;
    }

    // Casting

    address :: port :: operator bool () const
    {
        return this->_value;
    }

    address :: port :: operator in_port_t & ()
    {
        return this->_value;
    }

    address :: port :: operator const in_port_t & () const
    {
        return this->_value;
    }

    // address

    // Constructors

    address :: address() : address(ip(), port())
    {
    }

    address :: address(const char * host, const uint16_t & port) : address((class ip){host}, (class port){port})
    {
    }

    address :: address(const class ip & ip, const class port & port)
    {
        this->_value.sin_family = AF_INET;
        this->_value.sin_addr = (const in_addr &) ip;
        this->_value.sin_port = (const in_port_t &) port;

        #ifdef __APPLE__
        memset(this->_value.sin_zero, '\0', sizeof(this->_value.sin_zero));
        #endif
    }

    address :: address(const sockaddr_in & value) : _value(value)
    {
    }

    // Getters

    class address :: ip address :: ip() const
    {
        return (class ip)(*this);
    }

    class address :: port address :: port() const
    {
        return (class port)(*this);
    }

    // Operators

    bool address :: operator == (const address & rho) const
    {
        return this->_value.sin_addr.s_addr == rho._value.sin_addr.s_addr && this->_value.sin_port == rho._value.sin_port;
    }

    bool address :: operator == (const sockaddr_in & rho) const
    {
        return this->_value.sin_addr.s_addr == rho.sin_addr.s_addr && this->_value.sin_port == rho.sin_port;
    }

    // Casting

    address :: operator bool () const
    {
        return this->_value.sin_addr.s_addr || this->_value.sin_port;
    }

    address :: operator sockaddr_in & ()
    {
        return this->_value;
    }

    address :: operator const sockaddr_in & () const
    {
        return this->_value;
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const class address :: ip & ip)
    {
        char buffer[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &ip, buffer, INET_ADDRSTRLEN);
        return out << buffer;
    }

    std :: ostream & operator << (std :: ostream & out, const class address :: port & port)
    {
        return out << ntohs((const in_port_t &) port);
    }

    std :: ostream & operator << (std :: ostream & out, const address & address)
    {
        return out << "[" << address.ip() << ":" << address.port() << "]";
    }

    // Static declarations

    class any any;
};
