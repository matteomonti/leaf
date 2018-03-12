// Forward declarations

namespace drop
{
    class pool;
};

#if !defined(__forward__) && !defined(__drop__network__pool__h)
#define __drop__network__pool__h

// Libraries

#include <memory>
#include <thread>
#include <unordered_map>
#include <deque>
#include <mutex>

// Includes

#include "queue.h"
#include "connection.hpp"
#include "drop/async/promise.hpp"
#include "drop/data/variant.hpp"
#include "drop/thread/channel.hpp"
#include "drop/chrono/time.hpp"

namespace drop
{
    class pool
    {
        // Settings

        struct settings
        {
            static constexpr interval timeout = 30_s;
            static constexpr interval interval = 1_s;
        };

    public:

        // Nested classes

        class connection
        {
            // Friends

            friend class pool;

            // Service nested classes

            class arc
            {
                // Friends

                friend class pool;
                friend class connection;

                // Members

                pool & _pool;
                std :: shared_ptr <:: drop :: connection :: arc> _connection;

            public:

                // Constructors

                arc(pool &, const :: drop :: connection &);

                // Destructor

                ~arc();
            };

            // Members

            std :: shared_ptr <arc> _arc;

            // Private constructors

            connection();
            connection(pool &, const :: drop :: connection &);

        public:

            // Methods

            promise <void> send(const buffer &) const;
            promise <buffer> receive() const;
        };

        // Service nested classes

        struct request
        {
            pool :: connection connection;
            promise <void> promise;
            queue :: type type;
            size_t version;
        };

        struct timeout
        {
            int descriptor;
            timestamp timeout;
            size_t version;
        };

        // Members

        queue _queue;

        channel <request> _new;
        std :: unordered_map <int, request> _pending;

        std :: deque <timeout> _timeouts;

        struct
        {
            int read;
            int write;
        } _wake;

        volatile bool _alive;

        size_t _version;

        std :: mutex _mutex;
        std :: thread _thread;

    public:

        // Constructors

        pool();

        // Destructors

        ~pool();

        // Methods

        connection bind(const :: drop :: connection &);

    private:

        // Private methods

        promise <void> send(const connection &, const buffer &);
        promise <buffer> receive(const connection &);

        void run();
        void wake();
    };
};

#endif
