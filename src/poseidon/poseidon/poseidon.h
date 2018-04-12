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

        // Friends

        friend class gossiper;

        // Members

        signer _signer;

        gossiper _gossiper;
        crawler _crawler;

        uint64_t _sequence;

        std :: mutex _mutex;

        typename settings :: dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

    public:

        // Constructors

        poseidon(const signer &, const std :: array <identifier, brahms :: settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &);

        // Methods

        void start();
        void publish(const buffer &);

    private:

        // Private methods

        void gossip(const statement &);
    };
};

#endif
