#ifndef __drop__data__buffer__hpp
#define __drop__data__buffer__hpp

// Includes

#include "buffer.h"

namespace drop
{
    // Methods

    template <typename vtype> void buffer :: accept(bytewise :: visitor <vtype> & visitor) const
    {
        (visitor << varint(this->_size)).update(this->_bytes, this->_size);
    }
};

#endif
