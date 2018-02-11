#ifndef __drop__data__buffer__hpp
#define __drop__data__buffer__hpp

// Includes

#include "buffer.h"

namespace drop
{
    // Methods

    template <typename vtype> void buffer :: accept(bytewise :: reader <vtype> & reader) const
    {
        (reader << varint(this->_size)).update(this->_bytes, this->_size);
    }
};

#endif
