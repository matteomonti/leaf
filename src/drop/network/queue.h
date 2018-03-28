// Forward declarations

namespace drop
{
    class queue;
};

#if !defined(__forward__) && !defined(__drop__network__queue___h)
#define __drop__network__queue___h

// Libraries

#include <type_traits>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <exception>
#include <assert.h>

#ifdef __APPLE__
#include <sys/event.h>
#endif

#ifdef __linux__
#include <sys/epoll.h>
#endif

// Includes

#include "drop/chrono/time.hpp"

namespace drop
{
    class queue
    {
    public:

        // Settings

        struct settings
        {
            static constexpr size_t buffer = 1024;
        };

        // Exceptions

        struct exceptions
        {
            class kevent_failed : public std :: exception
            {
                const char * what() const noexcept;
            };

            class epoll_ctl_failed : public std :: exception
            {
                const char * what() const noexcept;
            };

            class epoll_wait_failed : public std :: exception
            {
                const char * what() const noexcept;
            };
        };

        // Nested enums

        enum type {write, read};

        // Nested classes

        class event : public
                            #ifdef __APPLE__
                            kevent
                            #endif
                            #ifdef __linux__
                            epoll_event
                            #endif
        {
        public:

            // Getters

            int descriptor() const;
            type type() const;
            bool error() const;
        };

    private:

        // Members

        int _descriptor;
        event _events[settings :: buffer];

    public:

        // Constructors

        queue();

        // Destructor

        ~queue();

        // Methods

        void add(const int &, const type &);
        void remove(const int &, const type &);

        size_t select();
        size_t select(const interval &);

        // Operators

        const event & operator [] (const size_t &) const;
    };
};

#endif
