#ifndef __drop__data__varint__hpp
#define __drop__data__varint__hpp

// Includes

#include "varint.h"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // Methods

    template <typename vtype> void varint :: accept(bytewise :: reader <vtype> & reader) const
    {
        if(this->_value < 128)
        {
            uint8_t buffer[1];
            buffer[0] = (uint8_t) this->_value;
            reader << buffer;
        }
        else if(this->_value < 16384)
        {
            uint8_t buffer[2];
            buffer[0] = (uint8_t)(this->_value >> 8) | 0x80;
            buffer[1] = (uint8_t)(this->_value);
            reader << buffer;
        }
        else
        {
            uint8_t buffer[4];
            buffer[0] = (uint8_t)(this->_value >> 24) | 0xc0;
            buffer[1] = (uint8_t)(this->_value >> 16);
            buffer[2] = (uint8_t)(this->_value >> 8);
            buffer[3] = (uint8_t)(this->_value);
            reader << buffer;
        }
    }

    template <typename vtype> void varint :: accept(bytewise :: writer <vtype> & writer)
    {
        uint8_t buffer[4];

        writer >> buffer[0];
        if(buffer[0] & 0x80)
        {
            writer >> buffer[1];

            if(buffer[0] & 0x40)
            {
                writer >> buffer[2] >> buffer[3];
                this->_value = ((uint32_t)(buffer[0] & 0x3f) << 24) | ((uint32_t)(buffer[1]) << 16) | ((uint32_t)(buffer[2]) << 8) | (uint32_t)(buffer[3]);
            }
            else
                this->_value = ((uint32_t)(buffer[0] & 0x7f) << 8) | (uint32_t)(buffer[1]);
        }
        else
            this->_value = buffer[0];
    }
};

#endif
