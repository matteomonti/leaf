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
#include "checkpool.hpp"
#include "drop/async/pipe.hpp"
#include "drop/network/pool.hpp"

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

        public:

            // Constructors

            server(const pool :: connection &, std :: unordered_map <index, vote, shorthash> &, std :: recursive_mutex &);

            // Getters

            bool alive();

            // Methods

            void push(const statement &);

        private:

            // Private methods

            promise <void> send();
            promise <void> receive();
        };

        class client
        {
        };
    };
};

#endif
