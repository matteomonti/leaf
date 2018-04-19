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
#include <unordered_map>

// Forward includes

#define __forward__
#include "poseidon.h"
#include "checker.h"
#undef __forward__

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
        // Friends

        friend class poseidon;
        friend class checker;

        // Members

        std :: unordered_set <index, shorthash> _indexes;
        std :: unordered_map <index, value, shorthash> _values[size];

        size_t _version;

        size_t _threshold;
        poseidon & _poseidon;

        // Constructors

        checkpool(poseidon &, const size_t &);

        // Getters

        std :: vector <index> indexes() const;

        // Methods

        void add(const index &);
        bool find(const index &);

        void set(const size_t &, const size_t &, const index &, const value &);

        size_t clear();
    };
};

#endif
