#ifndef __drop__bytewise__endianess__hpp
#define __drop__bytewise__endianess__hpp

// Includes

#include "endianess.h"

namespace drop
{
    // Static methods

    inline int8_t endianess :: swap(const int8_t & value)
    {
        return value;
    }

    inline uint8_t endianess :: swap(const uint8_t & value)
    {
        return value;
    }

    inline int16_t endianess :: swap(const int16_t & value)
    {
        return __builtin_bswap16(value);
    }

    inline uint16_t endianess :: swap(const uint16_t & value)
    {
        return __builtin_bswap16(value);
    }

    inline int32_t endianess :: swap(const int32_t & value)
    {
        return __builtin_bswap32(value);
    }

    inline uint32_t endianess :: swap(const uint32_t & value)
    {
        return __builtin_bswap32(value);
    }

    inline int64_t endianess :: swap(const int64_t & value)
    {
        return __builtin_bswap64(value);
    }

    inline uint64_t endianess :: swap(const uint64_t & value)
    {
        return __builtin_bswap64(value);
    }

    template <endianess :: type target, typename vtype, std :: enable_if_t <std :: is_integral <vtype> :: value> *> inline vtype endianess :: to(const vtype & value)
    {
        if constexpr(local != target)
            return swap(value);
        else
            return value;
    }

    template <typename vtype, std :: enable_if_t <std :: is_integral <vtype> :: value> *> inline vtype endianess :: translate(const vtype & value)
    {
        return to <network> (value);
    }
};

#endif
