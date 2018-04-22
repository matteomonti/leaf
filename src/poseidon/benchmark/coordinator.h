// Forward declarations

namespace poseidon
{
    class coordinator;
};

#if !defined(__forward__) && !defined(__poseidon__benchmark__coordinator__h)
#define __poseidon__benchmark__coordinator__h

// Libraries

#include <unordered_set>
#include <vector>
#include <mutex>
#include <array>

// Includes

#include "drop/network/acceptors/tcp.h"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/crypto/shorthash.hpp"
#include "vine/network/dial.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class coordinator
    {
        // Settings

        struct settings
        {
            struct view
            {
                static constexpr size_t size = 8;
            };

            struct intervals
            {
                static constexpr interval poll = 1_s;
            };
        };

        // Members

        size_t _nodes;

        struct
        {
            std :: unordered_set <identifier, shorthash> set;
            std :: vector <identifier> vector;
        } _identifiers;

        acceptors :: tcp :: async _acceptor;
        pool _pool;

        std :: mutex _mutex;

    public:

        // Constructors

        coordinator(const uint16_t &, const size_t &);

    private:

        // Private methods

        promise <void> serve(pool :: connection);

    public:

        // Static methods

        static std :: array <identifier, settings :: view :: size> await(const address &, const identifier &);
    };
};

#endif
