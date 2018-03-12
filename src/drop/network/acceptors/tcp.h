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
#include "drop/utils/sfinae.h"

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

            sync(const uint16_t &);

            // Getters

            const class address :: port & port() const;

            // Methods

            connection accept();
        };

        class async
        {
            // Settings

            struct settings
            {
                static constexpr size_t callbacks = 16;
            };

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

            class callback_interface
            {
            public:

                // Destructor

                virtual ~callback_interface();

                // Operators

                virtual void operator () (const connection &) = 0;
            };

            template <typename lambda> class callback : public callback_interface
            {
                // Members

                lambda _callback;

            public:

                // Constructors

                callback(const lambda &);

                // Operators

                void operator () (const connection &);
            };

            // Members

            sync _acceptor;
            callback_interface * _callbacks[settings :: callbacks];

            volatile bool _alive;
            std :: thread _thread;

        public:

            // Constructors

            async(const uint16_t &);

            // Destructor

            ~async();

            // Getters

            const class address :: port & port() const;

            // Methods

            template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, connection> :: value) && (constraints :: callback <lambda> ())> * = nullptr> void on(const lambda &);

            void reset();
            
        private:

            // Private methods

            void run();
            void wake();
        };
    };
};

#endif
