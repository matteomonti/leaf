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
        // Settings

        struct settings
        {
            struct view
            {
                static constexpr size_t size = 64;
            };

            struct sample
            {
                static constexpr size_t size = 16;
            };

            static constexpr size_t alpha = 24;
            static constexpr size_t beta = 56;
            static constexpr size_t gamma = sample :: size - (alpha + beta);
        };

    public:

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

        signature :: publickey _view[settings :: view :: size];
        sampler _sample[settings :: sample :: size];

        signer _signer;

        std :: mutex _mutex;

        directory :: client _directory;
        connectors :: tcp :: async & _connector;
        pool & _pool;
        crontab & _crontab;

    public:

        // Constructors

        brahms(const address &, connectors :: tcp :: async &, pool &, crontab &);
        brahms(const signer &, const address &, connectors :: tcp :: async &, pool &, crontab &);

        // Getters

        const signature :: publickey & identifier() const;
        signer & signer();

        // Methods

        void lock();
        void unlock();

    private:

        // Private methods

        void dispatch(const signature :: publickey &);
    };
};

#endif
