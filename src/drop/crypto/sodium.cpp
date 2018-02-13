// Includes

#include "sodium.h"

namespace drop
{
    // Exceptions

    const char * sodium :: exceptions :: init_failed :: what() const throw()
    {
        return "Failed to initialize the sodium library.";
    }

    // Private constructors

    sodium :: sodium()
    {
        if(sodium_init() < 0)
            throw exceptions :: init_failed();
    }

    // Static members declarations

    sodium sodium :: singleton;
};
