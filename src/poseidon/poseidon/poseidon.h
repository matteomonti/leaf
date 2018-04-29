// Forward declarations

namespace poseidon
{
    class poseidon;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__poseidon__h)
#define __poseidon__poseidon__poseidon__h

// Libraries

#include <unordered_set>
#include <unordered_map>

// Includes

#include "poseidon/settings.h"
#include "statement.h"
#include "gossiper.h"
#include "poseidon/brahms/brahms.h"
#include "drop/distributed/publisher.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class poseidon
    {
        // Typedefs

        typedef multiplexer <dialers :: local :: client, settings :: channels> dialer;

        // Members

        signer _signer;

        brahms _brahms;
        gossiper _gossiper;

        std :: unordered_map <index, vote, shorthash> _votes;
        std :: unordered_map <index, value, shorthash> _sample[settings :: sample :: size];
        std :: unordered_set <index, shorthash> _pending;

        publisher <index, value> _publisher;
        subscriber <index, value> * _subscribers[settings :: sample :: size];

        dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        std :: mutex _mutex;

        std :: ostream & log;

    public:

        // Constructors

        poseidon(const signer &, const view &, dialer &, pool &, crontab &, std :: ostream &);

        // Methods

        void start();
        void publish(const size_t &, const buffer &);

    private:

        promise <void> subscribe(size_t);

        void quorum(const index &);
    };
};

#endif
