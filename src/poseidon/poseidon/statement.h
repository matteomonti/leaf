// Forward declarations

namespace poseidon
{
    class statement;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__statement__h)
#define __poseidon__poseidon__statement__h

// Includes

#include "drop/data/buffer.hpp"
#include "vine/network/dial.h"
#include "drop/crypto/signature.hpp"
#include "drop/crypto/hash.hpp"
#include "drop/bytewise/bytewise.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class statement
    {
        // Signatures

        struct signatures
        {
            static const hash statement;
        };

        // Members

        identifier _identifier;
        uint64_t _sequence;
        buffer _value;
        signature _signature;

    public:

        // Constructors

        statement();
        statement(signer &, const uint64_t &, const buffer &);

        // Getters

        const identifier & identifier() const;
        const uint64_t & sequence() const;
        const buffer & value() const;
        const signature & signature() const;

        // Methods

        void verify() const;

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        bool operator == (const statement &) const;
    };
};

#endif
