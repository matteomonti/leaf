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
    template <typename type> class gossiper
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

        // Nested classes

        class handle
        {
            // Friends

            template <typename> friend class gossiper;

            // Members

            size_t _nonce;
            gossiper <type> * _gossiper;

        public:

            // Constructors

            handle();

        private:

            // Private constructors

            handle(const size_t &, gossiper <type> &);

        public:

            // Getters

            bool alive() const;

            // Methods

            void close() const;

            // Operators

            bool operator == (const handle &) const;
            bool operator != (const handle &) const;

            // Casting

            operator bool () const;
        };

    private:

        // Service nested classes

        class emitter : public eventemitter <type, handle, type>
        {
            // Friends

            template <typename> friend class gossiper;
        };

        // Static asserts

        static_assert(bytewise :: constraints :: serializable <type> (), "Gossiper can only gossip serializable types.");

        // Members

        syncset <type> _syncset;
        std :: unordered_set <type, shorthash> _addbuffer;
        std :: unordered_map <size_t, messenger <type>> _messengers;

        size_t _locks;

        size_t _nonce;
        std :: recursive_mutex _mutex;

        emitter _emitter;
        crontab & _crontab;

    public:

        // Constructors

        gossiper(crontab &);

        // Methods

        void publish(const type &);
        handle serve(const pool :: connection &, const bool &);

        template <typename etype, typename lambda, std :: enable_if_t <(std :: is_same <etype, type> :: value) && (eventemitter <type, handle, type> :: constraints :: template callback <lambda> ())> * = nullptr> void on(const lambda &);

    private:

        // Private methods

        void lock();
        void unlock();

        promise <void> serve(size_t, pool :: connection, bool);
        promise <void> sync(size_t, pool :: connection, bool);

        void gossip(const size_t &, const type &);
        void merge();
    };
};

#endif
