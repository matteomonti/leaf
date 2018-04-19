// Forward declarations

namespace poseidon
{
    class checker;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__checker__h)
#define __poseidon__poseidon__checker__h

// Libraries

#include <unordered_set>
#include <mutex>

// Includes

#include "statement.hpp"
#include "checkpool.h"
#include "drop/async/pipe.hpp"
#include "drop/network/pool.hpp"
#include "poseidon/brahms/brahms.h"

namespace poseidon
{
    using namespace drop;

    class checker
    {
    public:

        // Nested classes

        class server
        {
            // Members

            pool :: connection _connection;
            std :: unordered_map <index, vote, shorthash> & _votes;

            std :: unordered_set <index, shorthash> _indexes;

            bool _sending;
            bool _receiving;

            pipe <statement> _pipe;
            std :: recursive_mutex & _mutex;

            std :: ostream & log;

        public:

            // Constructors

            server(const pool :: connection &, std :: unordered_map <index, vote, shorthash> &, std :: recursive_mutex &, std :: ostream &);

            // Getters

            bool alive();

            // Methods

            void push(const statement &);

        private:

            // Private methods

            void serve(const index &);

            promise <void> send();
            promise <void> receive();
        };

        class client
        {
            // Members

            pool :: connection _connection;

            size_t _version;
            size_t _slot;

            volatile bool _alive;
            promise <void> _close;

            checkpool <brahms :: settings :: sample :: size> & _checkpool;

            pipe <index> _pipe;
            std :: recursive_mutex & _mutex;

            std :: ostream & log;

        public:

            // Constructors

            client(const pool :: connection &, const size_t &, const size_t &, const std :: vector <index> &, checkpool <brahms :: settings :: sample :: size> &, std :: recursive_mutex &, std :: ostream &);

            // Methods

            void push(const index &);
            promise <void> close();

            // Private methods

            promise <void> send(const std :: vector <index> &);
            promise <void> receive();
        };
    };
};

#endif
