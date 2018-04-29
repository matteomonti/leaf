// Forward declarations

namespace poseidon
{
    class gossiper;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__gossiper__h)
#define __poseidon__poseidon__gossiper__h

// Libraries

#include <unordered_map>

// Includes

#include "drop/distributed/gossiper.hpp"
#include "statement.hpp"
#include "poseidon/brahms/brahms.h"
#include "drop/crypto/shorthash.hpp"

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

        std :: unordered_map <identifier, int32_t, shorthash> _scores;

        dialer & _dialer;
        crontab & _crontab;

        std :: mutex _mutex;

    public:

        // Constructors

        gossiper(brahms &, dialer &, crontab &);

    private:

        // Private methods

        promise <void> serve(pool :: connection);
        promise <void> maintain(size_t);
        promise <void> ban();
    };
};

#endif
