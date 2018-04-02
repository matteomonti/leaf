// Forward declarations

namespace vine
{
    namespace dialers
    {
        class local;
    };
};

#if !defined(__forward__) && !defined(__vine__dialers__local__h)
#define __vine__dialers__local__h

// Libraries

#include <unordered_map>

// Includes

#include "vine/network/dial.h"
#include "drop/crypto/shorthash.hpp"
#include "drop/network/sockets/local.hpp"
#include "drop/network/pool.hpp"
#include "drop/thread/semaphore.h"
#include "drop/async/eventemitter.hpp"
#include "exceptions.h"

namespace vine :: dialers
{
    using namespace drop;

    class local
    {
    public:

        // Nested classes

        // Forward declarations

        class server;
        class client;

        class server
        {
            // Friends

            friend class client;

            // Members

            std :: unordered_map <identifier, client *, shorthash> _clients;
            std :: mutex _mutex;

            // Private methods

            void add(const identifier &, client *);
            void remove(const identifier &);

            promise <dial> connect(const identifier &, const identifier &);
        };

        class client : public eventemitter <dial, dial>
        {
            // Friends

            friend class server;

            // Members

            server & _server;
            signer _signer;
            keyexchanger _keyexchanger;

        public:

            // Constructors

            client(server &);
            client(server &, const signer &);
            client(server &, const signer &, const keyexchanger &);

            // Destructor

            ~client();

            // Getters

            const identifier & identifier() const;
            signer & signer();
            keyexchanger & keyexchanger();

            // Methods

            promise <dial> connect(const vine :: identifier &);
        };
    };
};

#endif
