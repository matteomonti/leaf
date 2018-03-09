#ifndef __drop__network__connection__hpp
#define __drop__network__connection__hpp

// Includes

#include "connection.h"

namespace drop
{
    // Constraints

    template <typename stype> constexpr bool connection :: constraints :: socket()
    {
        return std :: is_same <stype, sockets :: tcp> :: value;
    }

    // arc

    // Constructors

    template <typename stype> connection :: arc :: arc(const stype & socket) : _socket(socket)
    {
    }

    // connection

    // Constructors

    template <typename stype, std :: enable_if_t <connection :: constraints :: socket <stype> ()> *> connection :: connection(const stype & socket) : _arc(new arc(socket))
    {
    }
};

#endif
