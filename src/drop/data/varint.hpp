#ifndef __drop__data__varint__hpp
#define __drop__data__varint__hpp

// Includes

#include "varint.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // Methods

    template <typename vtype> void varint :: accept(bytewise :: visitor <vtype> & visitor) const
    {
        if(this->_value < 128)
        {
            uint8_t buffer[1];
            buffer[0] = (uint8_t) this->_value;
            visitor << buffer;
        }
        else if(this->_value < 16384)
        {
            uint8_t buffer[2];
            buffer[0] = (uint8_t)(this->_value >> 8) | 0x80;
            buffer[1] = (uint8_t)(this->_value);
            visitor << buffer;
        }
        else
        {
            uint8_t buffer[4];
            buffer[0] = (uint8_t)(this->_value >> 24) | 0xc0;
            buffer[1] = (uint8_t)(this->_value >> 16);
            buffer[2] = (uint8_t)(this->_value >> 8);
            buffer[3] = (uint8_t)(this->_value);
            visitor << buffer;
        }
    }
};

#endif
