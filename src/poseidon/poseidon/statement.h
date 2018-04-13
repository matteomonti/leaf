// Forward declarations

namespace poseidon
{
    class index;
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

    class index
    {
        // Members

        identifier _identifier;
        uint64_t _sequence;

    public:

        // Constructors

        index();
        index(const identifier &, const uint64_t &);

        // Getters

        const identifier & identifier() const;
        const uint64_t & sequence() const;

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        bool operator == (const index &) const;
    };

    struct value
    {
        // Members

        buffer value;
        signature signature;

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        bool operator == (const struct value &) const;
    };

    class statement
    {
        // Signatures

        struct signatures
        {
            static const hash statement;
        };

        // Members

        index _index;
        buffer _value;
        signature _signature;

    public:

        // Constructors

        statement();
        statement(signer &, const uint64_t &, const buffer &);
        statement(const index &, const buffer &, const signature &);
        statement(const identifier &, const uint64_t &, const buffer &, const signature &);

        // Getters

        const index & index() const;
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
        bool operator != (const statement &) const;
    };
};

#endif
