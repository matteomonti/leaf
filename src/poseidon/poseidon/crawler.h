// Forward declarations

namespace poseidon
{
    class crawler;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__crawler__h)
#define __poseidon__poseidon__crawler__h

// Libraries

#include <array>
#include <unordered_map>
#include <unordered_set>

// Forward includes

#define __forward__
#include "gossiper.h"
#undef __forward__

// Includes

#include "poseidon/brahms/brahms.h"
#include "drop/crypto/shorthash.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class crawler
    {
        // Settings

        struct settings
        {
            typedef multiplexer <dialers :: local :: client, 3> dialer;
            static constexpr size_t channel = 0;
            
            typedef gossiper server;

            struct intervals
            {
                static constexpr interval window = 10_m;
                static constexpr interval retry = 20_s;
            };

            struct thresholds
            {
                static constexpr uint64_t push = 3;
                static constexpr uint64_t sample = 2;
            };
        };

        // Members

        signer _signer;
        brahms _brahms;

        std :: unordered_map <vine :: identifier, uint64_t, shorthash> _scores;
        std :: unordered_set <vine :: identifier, shorthash> _connections;
        std :: unordered_multiset <vine :: identifier, shorthash> _neighborhood;

        std :: mutex _mutex;

        settings :: server & _server;

        typename settings :: dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

    public:

        // Constructors

        crawler(const std :: array <identifier, brahms :: settings :: view :: size> &, settings :: server &, typename settings :: dialer &, pool &, crontab &);
        crawler(const signer &, const std :: array <identifier, brahms :: settings :: view :: size> &, settings :: server &, typename settings :: dialer &, pool &, crontab &);

        // Getters

        const identifier & identifier() const;
        signer & signer();

        // Methods

        void start();

    private:
    public: // REMOVE ME

        // Private methods

        uint64_t score(const vine :: identifier &);
        void increment(const vine :: identifier &);
        void decrement(const vine :: identifier &);

        promise <void> serve(vine :: identifier, pool :: connection);
        promise <void> poll(vine :: identifier);

        promise <void> maintain(vine :: identifier);
        void drop(const vine :: identifier &);
    };
};

#endif
