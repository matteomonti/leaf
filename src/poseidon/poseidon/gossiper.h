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
#include <unordered_map>
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
#include "drop/async/pipe.hpp"

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
                static constexpr interval merge = 1_m;
                static constexpr interval retry = 2_s;
                static constexpr interval sync = 30_s;
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

        // Service nested classes

        class syncer
        {
            // Members

            gossiper & _gossiper;
            pool :: connection _connection;
            pipe <statement> _pipe;

            const std :: vector <statement> & _addbuffer;

            volatile bool _alive;
            promise <void> _close;

            std :: ostream & log;

        public:

            // Constructors

            syncer(gossiper &, const pool :: connection &, const std :: vector <statement> &, std :: ostream &);

            // Methods

            void push(const statement &);
            promise <void> close();

        private:

            // Private methods

            promise <void> send();
            promise <void> receive();
        };

        // Members

        identifier _identifier;
        syncset <statement> _statements;

        std :: unordered_set <statement, shorthash> _addbuffer;
        timestamp _nextmerge;

        std :: unordered_map <identifier, syncer *, shorthash> _syncers;

        size_t _locks;
        std :: recursive_mutex _mutex;

        typename settings :: handler & _handler;

        crontab & _crontab;

        std :: ostream & log;

    public:

        // Constructors

        gossiper(const identifier &, typename settings :: handler &, crontab &, std :: ostream &);

        // Methods

        void add(const statement &);

    private:

        // Private methods

        void lock();
        void unlock();

        promise <void> wait_merge();

        bool locked();
        bool merging();

        promise <void> serve(identifier, pool :: connection, bool);
        promise <void> run();
    };
};

#endif
