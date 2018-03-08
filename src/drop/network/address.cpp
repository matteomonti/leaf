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
        this->_ip.s_addr = htonl(INADDR_ANY);
    }

    address :: ip :: ip(const char * host)
    {
        hostent * record = gethostbyname(host);

        if(record)
            memcpy(&(this->_ip), record->h_addr, sizeof(in_addr));
        else
            throw exceptions :: host_not_found();
    }

    /*address :: ip :: ip(const address & address)
    {
        this->_ip.s_addr = address._address.sin_addr.s_addr;
    }*/

    bool address :: ip :: operator == (const ip & rho) const
    {
        return this->_ip.s_addr == rho._ip.s_addr;
    }

    // Casting

    address :: ip :: operator bool () const
    {
        return this->_ip.s_addr;
    }

    address :: ip :: operator in_addr & ()
    {
        return this->_ip;
    }

    address :: ip :: operator const in_addr & () const
    {
        return this->_ip;
    }
    
    // Static declarations

    class any any;
};
