// Forward declarations

namespace poseidon
{
    class gossiper;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__gossiper__h)
#define __poseidon__poseidon__gossiper__h

// Libraries

#include <unordered_map>
#include <unordered_set>

// Includes

#include "drop/async/eventemitter.hpp"
#include "drop/distributed/gossiper.hpp"
#include "statement.hpp"
#include "poseidon/brahms/brahms.h"
#include "drop/crypto/shorthash.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class gossiper : public eventemitter <statement, statement>
    {
        // Typedefs

        typedef multiplexer <dialers :: directory :: client, settings :: channels> dialer;

        // Members

        signer _signer;

        brahms & _brahms;
        drop :: gossiper <statement> _gossiper;

        std :: unordered_map <identifier, int32_t, shorthash> _scores;
        std :: unordered_set <typename drop :: gossiper <statement> :: id> _blacklist;

        dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        std :: mutex _mutex;

    public:

        // Constructors

        gossiper(const signer &, brahms &, dialer &, pool &, crontab &);

        // Methods

        void start();
        void publish(const statement &);

    private:

        // Private methods

        promise <void> serve(pool :: connection, identifier, bool);
        promise <void> maintain(bool, size_t);
        promise <void> ban();
    };
};

#endif
