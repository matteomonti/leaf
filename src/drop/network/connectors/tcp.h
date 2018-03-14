// Forward declarations

namespace drop
{
    namespace connectors
    {
        class tcp;
    };
};

#if !defined(__forward__) && !defined(__drop__network__connectors__h)
#define __drop__network__connectors__h

// Libraries

#include <thread>
#include <exception>
#include <unordered_map>
#include <deque>

#include "drop/network/address.h"
#include "drop/network/sockets/tcp.hpp"
#include "drop/network/connection.hpp"
#include "drop/thread/channel.hpp"
#include "drop/thread/wakepipe.h"
#include "drop/async/promise.hpp"
#include "drop/network/queue.h"
#include "drop/chrono/time.hpp"
#include "drop/network/sockets/exceptions.h"

namespace drop :: connectors
{
    class tcp
    {
    public:

        // Nested classes

        class sync
        {
        public:

            // Static methods

            static connection connect(const address &);
        };

        class async
        {
            // Settings

            struct settings
            {
                static constexpr interval timeout = 3_s;
                static constexpr interval interval = 1_s;
            };

        private:

            // Service nested classes

            struct request
            {
                sockets :: tcp socket;
                promise <connection> promise;
                size_t version;
            };

            struct timeout
            {
                // Public members

                int descriptor;
                timestamp timeout;
                size_t version;
            };

            // Members

            queue _queue;

            volatile bool _alive;
            size_t _version;

            channel <request> _new;
            std :: unordered_map <int, request> _pending;

            std :: deque <timeout> _timeouts;

            wakepipe _wakepipe;

            std :: mutex _mutex;
            std :: thread _thread;

        public:

            // Constructors

            async();

            // Destructor

            ~async();

            // Methods

            promise <connection> connect(const address &);

        private:

            // Private methods

            void run();
        };
    };
};

#endif
