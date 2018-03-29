// Forward declarations

namespace vine
{
    namespace dialers
    {
        class directory;
    };
};

#if !defined(__forward__) && !defined(__vine__dialers__directory__h)
#define __vine__dialers__directory__h

// Libraries

#include <unordered_map>

// Includes

#include "drop/network/address.hpp"
#include "drop/network/acceptors/tcp.h"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/crypto/keyexchanger.h"
#include "drop/crypto/signature.hpp"
#include "drop/crypto/hash.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/chrono/time.hpp"
#include "drop/chrono/crontab.h"
#include "drop/utils/sfinae.hpp"
#include "vine/network/dial.h"
#include "exceptions.h"

namespace vine :: dialers
{
    using namespace drop;

    class directory
    {
        // Signatures

        struct signatures
        {
            static const hash entry;
        };

    public:

        // Nested classes

        class server
        {
        public:

            // Settings

            struct settings
            {
                static constexpr interval timeout = 30_m;
            };

        private:

            // Service nested classes

            struct entry
            {
                address address;
                class keyexchanger :: publickey publickey;
                timestamp timestamp;
                signature signature;
            };

            // Members

            std :: unordered_map <identifier, entry, shorthash> _front;
            std :: unordered_map <identifier, entry, shorthash> _back;

            timestamp _last_swap;

            acceptors :: tcp :: async _acceptor;
            pool _pool;

        public:

            // Constructors

            server(const uint16_t &);

        private:

            // Private methods

            promise <void> serve(pool :: connection);
        };

        class client
        {
            // Settings

            struct settings
            {
                static constexpr interval timeout = server :: settings :: timeout;
                static constexpr interval refresh = 15_m;
            };

            // Traits

            struct traits
            {
                template <typename ctype> static constexpr bool is_dial_callable();
            };

        public:

            // Constraints

            struct constraints
            {
                template <typename ctype> static constexpr bool callback();
            };

        private:

            // Service nested classes

            struct entry
            {
                address address;
                class keyexchanger :: publickey publickey;
                timestamp timestamp;
            };

            // Members

            std :: unordered_map <identifier, entry, shorthash> _cache;

            address _server;

            signer _signer;
            keyexchanger _keyexchanger;

            acceptors :: tcp :: async _acceptor;

            connectors :: tcp :: async & _connector;
            pool & _pool;
            crontab & _crontab;

        public:

            // Constructors

            client(const address &, connectors :: tcp :: async &, pool &, crontab &);
            client(const address &, const signer &, connectors :: tcp :: async &, pool &, crontab &);
            client(const address &, const signer &, const keyexchanger &, connectors :: tcp :: async &, pool &, crontab &);

            // Getters

            const identifier & identifier() const;
            signer & signer();
            keyexchanger & keyexchanger();

            // Methods

            promise <dial> connect(vine :: identifier);
            template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, dial> :: value) && (constraints :: callback <lambda> ())> * = nullptr> void on(const lambda &);

        private:

            // Private methods

            promise <entry> lookup(vine :: identifier);
            promise <void> refresh();
        };
    };
}

#endif
