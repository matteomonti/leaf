// Forward declarations

namespace poseidon
{
    class brahms;
};

#if !defined(__poseidon__brahms__brahms__h)
#define __poseidon__brahms__brahms__h

// Libraries

#include <mutex>

// Includes

#include "sampler.h"
#include "poseidon/network/directory.hpp"

namespace poseidon
{
    using namespace drop;

    class brahms
    {
    public:

        // Settings

        struct settings
        {
            struct view
            {
                static constexpr size_t size = 32;
            };

            struct sample
            {
                static constexpr size_t size = 8;
            };

            static constexpr size_t alpha = 12;
            static constexpr size_t beta = 28;
            static constexpr size_t gamma = sample :: size - (alpha + beta);

            static constexpr interval interval = 10_s;
        };

        // Nested classes

        class node
        {
            // Friends

            friend class brahms;

            // Members

            signature :: publickey _identifier;
            brahms & _brahms;

            // Private constructors

            node(const signature :: publickey &, brahms &);

        public:

            // Methods

            promise <directory :: connection> connect() const;
        };

    private:

        // Members

        signer _signer;

        signature :: publickey _view[settings :: view :: size];
        sampler _sample[settings :: sample :: size];

        std :: mutex _mutex;

        directory :: client _directory;
        connectors :: tcp :: async & _connector;
        pool & _pool;
        crontab & _crontab;

    public:

        // Public members

        bool log = false; // REMOVE ME

        // Constructors

        brahms(const signature :: publickey (&)[settings :: view :: size], const address &, connectors :: tcp :: async &, pool &, crontab &);
        brahms(const signer &, const signature :: publickey (&)[settings :: view :: size], const address &, connectors :: tcp :: async &, pool &, crontab &);

        // Getters

        const signature :: publickey & identifier() const;
        signer & signer();

        // Methods

        void lock();
        void unlock();

    private:

        // Private methods

        void dispatch(const signature :: publickey &);
        promise <void> run();
    };
};

#endif
