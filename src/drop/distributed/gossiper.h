// Forward declarations

namespace drop
{
    template <typename> class gossiper;
};

#if !defined(__forward__) && !defined(__drop__distributed__gossiper__h)
#define __drop__distributed__gossiper__h

// Libraries

#include <unordered_set>
#include <mutex>

// Includes

#include "messenger.hpp"
#include "drop/data/syncset.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/async/eventemitter.hpp"

namespace drop
{
    template <typename type> class gossiper : public eventemitter <type, uint64_t, type>
    {
        // Settings

        struct settings
        {
            struct buffer
            {
                static constexpr size_t failsafe = 4096;
            };
        };

    public:

        // Exceptions

        struct exceptions
        {
            class buffer_overflow : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Typedefs

        typedef uint64_t id;

        // Nested classes

        class handle : public promise <void>
        {
            // Friends

            template <typename> friend class gossiper;

            // Members

            id _id;
            gossiper <type> * _gossiper;

        public:

            // Constructors

            handle() = default;

        private:

            // Private constructors

            handle(const id &, const promise <void> &, gossiper <type> &);

        public:

            // Getters

            bool alive() const;

            // Methods

            void close() const;

            // Casting

            operator const id & () const;
        };

    private:

        // Service nested classes

        struct messenger
        {
            drop :: messenger <type> messenger;
            promise <void> promise;
        };

        // Static asserts

        static_assert(bytewise :: constraints :: serializable <type> (), "Gossiper can only gossip serializable types.");

        // Members

        syncset <type> _syncset;
        std :: unordered_set <type, shorthash> _addbuffer;
        std :: unordered_map <id, messenger> _messengers;

        size_t _locks;

        id _nonce;
        std :: recursive_mutex _mutex;

        crontab & _crontab;

    public:

        // Constructors

        gossiper(crontab &);

        // Methods

        void publish(const type &);
        handle serve(const pool :: connection &, const bool &);

    private:

        // Private methods

        void lock();
        void unlock();

        promise <void> serve(id, pool :: connection, bool, promise <void>);
        promise <void> sync(id, pool :: connection, bool);

        void gossip(const id &, const type &);
        void merge();
        void drop(const id &);
    };
};

#endif
