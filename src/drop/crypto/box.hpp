#ifndef __drop__crypto__box__hpp
#define __drop__crypto__box__hpp

// Includes

#include "box.h"

namespace drop
{
    // publickey

    // Methods

    template <typename vtype> void box :: publickey :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void box :: publickey :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }

    // secretkey

    template <typename vtype> void box :: secretkey :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void box :: secretkey :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }
};

#endif
