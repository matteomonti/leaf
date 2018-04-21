// Includes

#include "promise.hpp"

namespace drop
{
    // Exceptions

    namespace exceptions
    {
        // multiple

        const char * multiple :: what() const throw()
        {
            return "Multiple exceptions.";
        }        
    };
};
