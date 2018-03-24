// Forward declarations

namespace poseidon
{
    class sampler;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__sampler__h)
#define __poseidon__brahms__sampler__h

// Includes

#include "drop/crypto/hash.hpp"
#include "drop/crypto/signature.hpp"
#include "drop/data/optional.hpp"

namespace poseidon
{
    using namespace drop;

    class sampler
    {
        // Members

        hash :: key _key;

        optional <signature :: publickey> _value;
        hash _hash;

    public:

        // Constructors

        sampler();

        // Getters

        const optional <signature :: publickey> & value() const;

        // Methods

        void init();
        void next(const signature :: publickey &);
    };
};

#endif
