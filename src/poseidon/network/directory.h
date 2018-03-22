// Forward declarations

namespace poseidon
{
    class directory;
};

#if !defined(__forward__) && !defined(__poseidon__network__directory__h)
#define __poseidon__network__directory__h

// Libraries

#include <unordered_map>

// Includes

#include "drop/network/address.hpp"
#include "drop/network/acceptors/tcp.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/network/pool.hpp"
#include "drop/crypto/keyexchanger.h"
#include "drop/crypto/signature.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/chrono/time.hpp"
#include "drop/chrono/crontab.h"
#include "drop/utils/sfinae.hpp"

namespace poseidon
{
    using namespace drop;

    class directory
    {
        // Signatures

        struct signatures
        {
            static const buffer entry;
        };

    public:

        // Nested classes

        class connection : public :: drop :: connection
        {
            // Members

            signature :: publickey _identifier;

        public:

            // Constructors

            connection(const :: drop :: connection &, const signature :: publickey &);

            // Getters

            const signature :: publickey & identifier() const;
        };

        class server
        {
            // Settings

            struct settings
            {
                static constexpr interval timeout = 30_m;
            };

            // Service nested classes

            struct entry
            {
                address address;
                class keyexchanger :: publickey publickey;
                signature signature;
            };

            // Members

            std :: unordered_map <signature :: publickey, entry, shorthash> _front;
            std :: unordered_map <signature :: publickey, entry, shorthash> _back;

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
                static constexpr interval recache = 30_m;
                static constexpr interval refresh = 15_m;
            };

        public:

            // Exceptions

            struct exceptions
            {
                class lookup_failed : public std :: exception
                {
                    const char * what() const throw();
                };
            };

        private:

            // Traits

            struct traits
            {
                template <typename ctype> static constexpr bool is_connection_callable();
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
                timestamp time;
            };

            // Members

            std :: unordered_map <signature :: publickey, entry, shorthash> _cache;

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

            const signature :: publickey & identifier() const;
            signer & signer();
            keyexchanger & keyexchanger();

            // Methods

            promise <connection> connect(signature :: publickey);
            template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, connection> :: value) && (constraints :: callback <lambda> ())> * = nullptr> void on(const lambda &);

        private:

            // Private methods

            promise <entry> lookup(signature :: publickey);
            promise <void> refresh();
        };
    };
};

#endif
