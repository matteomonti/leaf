// Includes

#include "varint.hpp"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    // varint

    // Constructors

    varint :: varint(const uint32_t & value) : _value(value)
    {
    }

    // Operators

    varint & varint :: operator = (const uint32_t & value)
    {
        this->_value = value;
        return (*this);
    }

    // Casting

    varint :: operator uint32_t & ()
    {
        return this->_value;
    }

    varint :: operator const uint32_t & () const
    {
        return this->_value;
    }

    // streamer

    // Constructors

    varint :: streamer :: streamer() : _bytes{}, _cursor(0)
    {
    }

    // Methods

    void varint :: streamer :: update(const uint8_t * bytes, const size_t & size)
    {
        memcpy(this->_bytes + this->_cursor, bytes, size);
        this->_cursor += size;
    }

    size_t varint :: streamer :: pending()
    {
        if(this->_cursor == 0)
            return 1;
        else if(this->_bytes[0] & 0x80)
        {
            if(this->_bytes[0] & 0x40)
                return (4 - this->_cursor);
            else
                return (2 - this->_cursor);
        }
        else
            return 0;
    }

    varint varint :: streamer :: yield()
    {
        return bytewise :: deserialize <varint> (this->_bytes, this->_cursor);
    }
};
