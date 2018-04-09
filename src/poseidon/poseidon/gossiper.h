// Forward declarations

namespace poseidon
{
    class gossiper;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__gossiper__h)
#define __poseidon__poseidon__gossiper__h

// Includes

#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"

namespace poseidon
{
    using namespace drop;

    class gossiper
    {
    public:

        // Members

        crontab & _crontab;

        // Constructors

        gossiper(crontab &);

        // Methods

        promise <void> serve(pool :: connection);
    };
};

#endif
