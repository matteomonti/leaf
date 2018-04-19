// Forward declarations

namespace poseidon
{
    namespace events
    {
        struct gossip;
        struct accept;
    };

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
#include "checker.h"
#include "checkpool.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/data/varint.hpp"
#include "drop/async/eventemitter.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    namespace events
    {
        struct gossip {};
        struct accept {};
    };

    class poseidon : public eventemitter <events :: gossip, statement>,
                     public eventemitter <events :: accept, statement>
    {
        // Settings

        struct settings
        {
            typedef multiplexer <dialers :: local :: client, 3> dialer;
            static constexpr size_t channel = 2;

            struct accept
            {
                static constexpr size_t threshold = 6;
            };
        };

    public:

        // Using

        using eventemitter <events :: gossip, statement> :: on;
        using eventemitter <events :: accept, statement> :: on;

    private:

        // Private using

        using eventemitter <events :: gossip, statement> :: emit;
        using eventemitter <events :: accept, statement> :: emit;

        // Friends

        friend class gossiper;

        // Members

        signer _signer;

        gossiper _gossiper;
        crawler _crawler;

        std :: unordered_map <index, vote, shorthash> _votes;
        checkpool <brahms :: settings :: sample :: size> _checkpool;

        std :: unordered_set <checker :: server *> _servers;

        std :: recursive_mutex _mutex;

        settings :: dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        std :: ostream & log;

    public:

        // Constructors

        poseidon(const signer &, const std :: array <identifier, brahms :: settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &, std :: ostream &);

        // Methods

        void start();
        void publish(const uint64_t &, const buffer &);

    private:

        // Private methods

        void gossip(const statement &);
        void serve(const pool :: connection &);
    };
};

#endif
