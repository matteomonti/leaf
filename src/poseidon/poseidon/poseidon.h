// Forward declarations

namespace poseidon
{
    class poseidon;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__poseidon__h)
#define __poseidon__poseidon__poseidon__h

// Libraries

#include <unordered_map>
#include <unordered_set>

// Includes

#include "crawler.h"
#include "gossiper.h"
#include "checkpool.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/data/varint.hpp"
#include "drop/async/eventemitter.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class poseidon : public eventemitter <statement, statement>
    {
        // Settings

        struct settings
        {
            typedef multiplexer <dialers :: local :: client, 3> dialer;
            static constexpr size_t channel = 2;

            struct intervals
            {
                static constexpr interval vote = 2_s;
                static constexpr interval check = 0.2_s;
            };

            struct accept
            {
                static constexpr size_t threshold = 6;
            };
        };

        // Friends

        friend class gossiper;

        // Service nested classes

        struct entry
        {
            buffer value;
            signature signature;
            timestamp timestamp;
            bool accepted;
        };

        // Members

        signer _signer;

        gossiper _gossiper;
        crawler _crawler;

        uint64_t _sequence;

        std :: unordered_map <index, entry, shorthash> _logs;
        std :: unordered_set <index, shorthash> _checklist;
        checkpool <brahms :: settings :: sample :: size> _checkpool;

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

        promise <void> serve(pool :: connection);
        promise <void> check(size_t, size_t, optional <identifier>, std :: vector <index>);
        promise <void> run();
    };
};

#endif
