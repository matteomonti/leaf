// Forward declarations

namespace poseidon
{
    template <size_t> class checkpool;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__checkpool__h)
#define __poseidon__poseidon__checkpool__h

// Libraries

#include <vector>
#include <unordered_set>

// Includes

#include "statement.hpp"
#include "drop/data/buffer.hpp"
#include "drop/crypto/shorthash.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    template <size_t size> class checkpool
    {
        // Members

        std :: vector <index> _indexes;
        std :: vector <optional <buffer>> _slots[size];

        size_t _version;

    public:

        // Methods

        size_t init(const std :: unordered_set <index, shorthash> &);
        void set(const size_t &, const size_t &, const std :: vector <optional <buffer>> &);
    };
};

#endif
