// Forward declarations

namespace poseidon
{
    class sampler;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__sampler__h)
#define __poseidon__brahms__sampler__h

// Includes

#include "poseidon/settings.h"
#include "drop/crypto/hash.hpp"
#include "drop/crypto/signature.hpp"
#include "drop/data/optional.hpp"
#include "vine/network/dial.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class sampler
    {
        // Members

        hash :: key _key;

        optional <identifier> _sample;
        hash _hash;

    public:

        // Constructors

        sampler();

        // Getters

        const optional <identifier> & sample() const;

        // Methods

        void init();
        bool next(const identifier &);
    };
};

#endif