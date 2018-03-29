// Includes

#include "exceptions.h"

namespace vine :: dialers
{
    // Exceptions

    const char * exceptions :: node_not_found :: what() const throw()
    {
        return "Lookup failed.";
    }
};
