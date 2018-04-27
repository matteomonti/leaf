// Forward declarations

namespace poseidon
{
    namespace staticsample
    {
        struct statement;
        class peer;
    };
};

#if !defined(__forward__) && !defined(__poseidon__benchmark__staticsample__h)
#define __poseidon__benchmark__staticsample__h

// Includes

#include "drop/distributed/gossiper.hpp"
#include "drop/distributed/publisher.hpp"
#include "drop/crypto/signature.hpp"
#include "vine/dialers/directory.hpp"

namespace poseidon :: staticsample
{
    using namespace drop;
    using namespace vine;

    struct statement
    {
        // Public members

        uint64_t id;
        buffer message;

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> & reader) const
        {
            reader << (this->id);
            reader << (this->message);
        }

        template <typename vtype> void accept(bytewise :: writer <vtype> & writer)
        {
            writer >> (this->id);
            writer >> (this->message);
        }

        // Operators

        bool operator == (const statement & rho) const
        {
            return (this->id == rho.id) && (this->message == rho.message);
        }

        bool operator != (const statement & rho) const
        {
            return !((*this) == rho);
        }
    };

    class peer
    {
        // Members

        size_t _id;

        signer _signer;

        interval _rate;

        std :: unordered_map <uint64_t, buffer> _statements;
        std :: unordered_map <uint64_t, size_t> _votes;

        connectors :: tcp :: async _connector;
        pool _pool;
        crontab _crontab;

        dialers :: directory :: client _dialer;

        publisher <uint64_t, buffer> _publisher;
        gossiper <statement> _gossiper;

        std :: vector <identifier> _sample;
        std :: vector <identifier> _view;

        subscriber <uint64_t, buffer> ** _subscribers;

        std :: mutex _mutex;

    public:

        // Constructors

        peer(const size_t &, const signer &, const interval &, const std :: vector <identifier> &, const std :: vector <identifier> &, const address &);

        // Methods

        void start();

    private:

        // Private methods

        promise <void> maintain_gossip(size_t);
        promise <void> connect_subscriber(size_t);

        promise <void> seed();
    };
}

#endif
