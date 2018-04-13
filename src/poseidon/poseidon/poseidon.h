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

            struct intervals
            {
                static constexpr interval vote = 5_m;
                static constexpr interval check = 1_m;
            };
        };

        // Friends

        friend class gossiper;

        // Service nested classes

        struct value
        {
            // Members

            buffer value;
            signature signature;

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);
        };

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

        std :: ostream & log;

    public:

        // Constructors

        poseidon(const signer &, const std :: array <identifier, brahms :: settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &, std :: ostream &);

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
