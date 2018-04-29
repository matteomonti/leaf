// Forward declarations

namespace poseidon
{
    class gossiper;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__gossiper__h)
#define __poseidon__poseidon__gossiper__h

// Includes

#include "drop/distributed/gossiper.hpp"
#include "statement.hpp"
#include "poseidon/brahms/brahms.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class gossiper
    {
        // Typedefs

        typedef multiplexer <dialers :: local :: client, settings :: channels> dialer;

        // Members

        brahms & _brahms;
        drop :: gossiper <statement> _gossiper;

        dialer & _dialer;

    public:

        // Constructors

        gossiper(brahms &, dialer &, crontab &);
    };
};

#endif
