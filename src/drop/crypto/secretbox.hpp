#ifndef __drop__crypto__secretbox__hpp
#define __drop__crypto__secretbox__hpp

// Includes

#include "secretbox.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"

namespace drop
{
    // key

    // Methods

    template <typename vtype> void secretbox :: key :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bytes);
    }

    template <typename vtype> void secretbox :: key :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bytes);
    }
};

#endif
