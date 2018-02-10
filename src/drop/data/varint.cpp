// Includes

#include "varint.hpp"

namespace drop
{
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
};
