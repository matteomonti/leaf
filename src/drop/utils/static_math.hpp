#ifndef __drop__utils__static_math__hpp
#define __drop__utils__static_math__hpp

// Includes

#include "static_math.h"

namespace drop
{
    template <typename type, size_t size> constexpr type max(const type (& values) [size])
    {
        type max = values[0];

        for(size_t i = 0; i < size; i++)
            if(max < values[i])
                max = values[i];

        return max;
    }
};

#endif
