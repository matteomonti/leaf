// Forward declarations

namespace poseidon
{
    class gossiper;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__gossiper__h)
#define __poseidon__poseidon__gossiper__h

// Libraries

#include <mutex>
#include <unordered_set>
#include <exception>

// Forward includes

#define __forward__
#include "poseidon.h"
#undef __forward__

// Includes

#include "drop/async/eventemitter.hpp"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "drop/chrono/time.hpp"
#include "drop/data/syncset.hpp"
#include "drop/crypto/shorthash.hpp"
#include "statement.hpp"

namespace poseidon
{
    using namespace drop;

    class gossiper
    {
        // Settings

        struct settings
        {
            typedef poseidon handler;

            struct intervals
            {
                static constexpr interval merge = 0.5_s;
                static constexpr interval retry = 0.02_s;
            };
        };

        // Friends

        friend class crawler;

        // Exceptions

        struct exceptions
        {
            class merge_in_progress : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Members

        identifier _identifier;
        syncset <statement> _statements;

        std :: unordered_set <statement, shorthash> _addbuffer;
        timestamp _nextmerge;

        size_t _locks;

        std :: mutex _mutex;

        typename settings :: handler & _handler;

        crontab & _crontab;

    public:

        // Constructors

        gossiper(const identifier &, typename settings :: handler &, crontab &);

        // Methods

        void add(const statement &);

    private:

        // Private methods

        void lock();
        void unlock();

        promise <void> wait_merge();

        bool locked();
        bool merging();

        promise <void> serve(identifier, pool :: connection);
        promise <void> run();
    };
};

#endif
