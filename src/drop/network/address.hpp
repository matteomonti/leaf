#ifndef __drop__network__address__hpp
#define __drop__network__address__hpp

// Includes

#include "address.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // ip

    // Methods

    template <typename vtype> void address :: ip :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (reinterpret_cast <const uint8_t (&)[sizeof(in_addr)]> (this->_value));
    }

    template <typename vtype> void address :: ip :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (reinterpret_cast <uint8_t (&)[sizeof(in_addr)]> (this->_value));
    }

    // port

    // Methods

    template <typename vtype> void address :: port :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (reinterpret_cast <const uint8_t (&)[sizeof(in_port_t)]> (this->_value));
    }

    template <typename vtype> void address :: port :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (reinterpret_cast <uint8_t (&)[sizeof(in_port_t)]> (this->_value));
    }

    // address

    // Methods

    template <typename vtype> void address :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (reinterpret_cast <const uint8_t (&)[sizeof(in_addr_t)]> (this->_value.sin_addr.s_addr));
        reader << (reinterpret_cast <const uint8_t (&)[sizeof(in_port_t)]> (this->_value.sin_port));
    }

    template <typename vtype> void address :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (reinterpret_cast <uint8_t (&)[sizeof(in_addr_t)]> (this->_value.sin_addr.s_addr));
        writer >> (reinterpret_cast <uint8_t (&)[sizeof(in_port_t)]> (this->_value.sin_port));
    }
};

#endif
