#ifndef __drop__network__connection__hpp
#define __drop__network__connection__hpp

// Includes

#include "connection.h"

namespace drop
{
    // arc

    // Constructors

    template <typename stype> connection :: arc :: arc(const stype & socket) : _socket(socket)
    {
    }
};

#endif
