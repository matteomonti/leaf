// Forward declarations

namespace drop
{
    class pool;
};

#if !defined(__forward__) && !defined(__drop__network__pool__h)
#define __drop__network__pool__h

// Libraries

#include <thread>
#include <exception>
#include <unordered_map>
#include <deque>
#include <memory>

// Includes

#include "connection.hpp"
#include "queue.h"

namespace drop
{
    class pool
    {
    public:

        // Nested classes

        class connection
        {
            // Friends

            friend class pool;

            // Members

            pool & _pool;
            std :: shared_ptr <:: drop :: connection :: arc> _arc;

            // Private constructors

            connection(pool &, const :: drop :: connection &);
        };

    public:

        // Methods

        connection bind(const :: drop :: connection &);
    };
};

#endif
