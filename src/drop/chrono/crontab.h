// Forward declarations

namespace drop
{
    class crontab;
};

#if !defined(__forward__) && !defined(__drop__chrono__crontab__h)
#define __drop__chrono__crontab__h

// Libraries

#include <thread>
#include <queue>

// Includes

#include "time.hpp"
#include "drop/async/promise.hpp"
#include "drop/thread/semaphore.h"

namespace drop
{
    class crontab
    {
        // Settings

        struct settings
        {
            struct bucket
            {
                static constexpr interval size = 0.1_s;
                static constexpr size_t count = 3600;
            };
        };

        // Service nested classes

        class event
        {
            // Members

            promise <void> _promise;
            timestamp _time;

        public:

            // Constructors

            event(const timestamp &);

            // Getters

            const promise <void> & promise() const;
            const timestamp & time() const;

            // Operators

            bool operator < (const event &) const;
            bool operator <= (const event &) const;

            bool operator > (const event &) const;
            bool operator >= (const event &) const;

            bool operator == (const event &) const;
            bool operator != (const event &) const;
        };

        class bucket : public std :: priority_queue <event>
        {
            // Members

            std :: recursive_mutex _mutex;

        public:

            // Methods

            void lock();
            void unlock();
        };

        // Members

        bucket * _buckets;
        volatile uint64_t _cursor;

        volatile bool _alive;
        semaphore _semaphore;

        std :: thread _thread;

    public:

        // Constructors

        crontab();

        // Destructor

        ~crontab();

        // Methods

        promise <void> wait(const timestamp &);
        promise <void> wait(const interval &);

    private:

        // Private methods

        void run();
    };
};

#endif
