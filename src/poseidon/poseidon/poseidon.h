// Forward declarations

namespace poseidon
{
    class poseidon;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__poseidon__h)
#define __poseidon__poseidon__poseidon__h

// Includes

#include "crawler.h"
#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class poseidon
    {
        // Settings

        struct settings
        {
            typedef multiplexer <dialers :: local :: client, 3> dialer;
            static constexpr size_t channel = 2;
        };

        // Members

        signer _signer;

        gossiper _gossiper;
        crawler _crawler;

        typename settings :: dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        // Constructors

        poseidon(const signer &, const std :: array <identifier, brahms :: settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &);
    };
};

#endif
