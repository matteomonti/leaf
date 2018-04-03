// Forward declarations

namespace poseidon
{
    class brahms;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__brahms__h)
#define __poseidon__brahms__brahms__h

// Libraries

#include <sodium.h>
#include <array>

// Includes

#include "sampler.h"
#include "drop/network/connectors/tcp.h"
#include "drop/chrono/crontab.h"
#include "vine/dialers/directory.hpp"
#include "vine/dialers/local.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    class brahms
    {
    public:

        // Settings

        struct settings
        {
            typedef dialers :: local :: client dialer;

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

        class node : public vine :: identifier
        {
            // Friends

            friend class brahms;

            // Members

            brahms & _brahms;

            // Private constructors

            node(const identifier &, brahms &);

        public:

            // Methods

            promise <dial> connect() const;
        };

    private:

        // Service nested classes

        struct pullslot
        {
            bool completed;
            std :: array <identifier, settings :: view :: size> view;
        };

        // Members

        signer _signer;

        std :: array <identifier, settings :: view :: size> _view;
        std :: array <sampler, settings :: sample :: size> _sample;

        std :: array <pullslot, settings :: alpha> _pullslots;
        size_t _version;

        std :: mutex _mutex;

        typename settings :: dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        std :: ostream & log; // REMOVE ME

    public:

        // Constructors

        brahms(const std :: array <identifier, settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &, std :: ostream &);
        brahms(const signer &, const std :: array <identifier, settings :: view :: size> &, typename settings :: dialer &, pool &, crontab &, std :: ostream &);

        // Getters

        const identifier & identifier() const;
        signer & signer();

    private:

        // Private methods

        void update_sample(const vine :: identifier &);

        promise <void> pull(vine :: identifier, size_t, size_t);
        promise <void> serve(pool :: connection);

        promise <void> run();
    };
};

#endif
