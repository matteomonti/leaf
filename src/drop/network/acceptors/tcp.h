// Forward declarations

namespace drop
{
    namespace acceptors
    {
        class tcp;
    };
};

#if !defined(__forward__) && !defined(__drop__network__acceptors__tcp__h)
#define __drop__network__acceptors__tcp__h

// Libraries

#include <stdint.h>
#include <type_traits>
#include <thread>
#include <mutex>

// Includes

#include "drop/network/sockets/tcp.h"
#include "drop/network/connection.hpp"
#include "drop/network/connectors/tcp.h"
#include "drop/async/eventemitter.hpp"

namespace drop :: acceptors
{
    class tcp
    {
    public:

        // Nested classes

        class sync
        {
            // Members

            sockets :: tcp _socket;

        public:

            // Constructors

            sync();
            sync(const uint16_t &);

            // Getters

            const class address :: port & port() const;

            // Methods

            connection accept();
        };

        class async : public eventemitter <connection, connection>
        {
            // Members

            sync _acceptor;

            volatile bool _alive;
            std :: thread _thread;

        public:

            // Constructors

            async();
            async(const uint16_t &);

            // Destructor

            ~async();

            // Getters

            const class address :: port & port() const;

        private:

            // Private methods

            void run();
            void wake();
        };
    };
};

#endif
