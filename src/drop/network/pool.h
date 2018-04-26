// Forward declarations

namespace drop
{
    class pool;
};

#if !defined(__forward__) && !defined(__drop__network__pool__h)
#define __drop__network__pool__h

// Libraries

#include <thread>
#include <exception>
#include <unordered_map>
#include <deque>
#include <memory>
#include <errno.h>
#include <mutex>

// Includes

#include "connection.hpp"
#include "queue.h"
#include "drop/thread/channel.h"
#include "drop/thread/wakepipe.h"
#include "drop/async/promise.h"
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

        // Exceptions

        struct exceptions
        {
            class event_error : public std :: exception
            {
                const char * what() const throw();
            };
        };

    public:

        // Nested classes

        class connection
        {
            // Friends

            friend class pool;

            // Members

            pool & _pool;
            std :: shared_ptr <:: drop :: connection :: arc> _arc;

            // Private constructors

            connection(pool &, const :: drop :: connection &);

        public:

            // Getters

            address remote() const;
            bool authenticated() const;

            // Methods

            promise <void> send(const buffer &) const;
            template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <types> ()))> * = nullptr> promise <void> send(const types & ...) const;

            promise <buffer> receive() const;
            template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <types> ()))> * = nullptr> auto receive() const;

            promise <void> authenticate(keyexchanger, class keyexchanger :: publickey) const;
        };

    private:

        // Service nested classes

        struct request
        {
            std :: shared_ptr <:: drop :: connection :: arc> arc;
            queue :: type type;

            size_t version;
            promise <void> promise;
        };

        struct timeout
        {
            int descriptor;
            queue :: type type;

            timestamp timeout;
            size_t version;
        };

        // Members

        queue _queue;

        volatile bool _alive;
        wakepipe _wakepipe;

        channel <request> _new;
        std :: mutex _newmtx;

        std :: unordered_map <int, request> _write;
        std :: unordered_map <int, request> _read;

        size_t _version;
        std :: deque <timeout> _timeouts;

        std :: thread _thread;

    public:

        // Constructors

        pool();

        // Destructor

        ~pool();

        // Methods

        connection bind(const :: drop :: connection &);

    private:

        // Private methods

        promise <void> send(const std :: shared_ptr <:: drop :: connection :: arc> &, const buffer &);
        promise <buffer> receive(const std :: shared_ptr <:: drop :: connection :: arc> &);

        void run();
    };
};

#endif
