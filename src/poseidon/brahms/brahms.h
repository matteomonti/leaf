// Forward declarations

namespace poseidon
{
    namespace events
    {
        struct push;
        struct view;
    };

    class brahms;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__brahms__h)
#define __poseidon__brahms__brahms__h

// Libraries

#include <sodium.h>
#include <array>
#include <vector>
#include <algorithm>

// Includes

#include "sampler.h"
#include "drop/network/connectors/tcp.h"
#include "drop/chrono/crontab.h"
#include "drop/data/optional.hpp"
#include "drop/async/eventemitter.hpp"
#include "vine/dialers/directory.hpp"
#include "vine/dialers/local.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    namespace events
    {
        struct push
        {
            struct send {};
            struct receive {};
        };

        struct view
        {
            struct join {};
            struct leave {};
        };
    };

    class brahms : public eventemitter <events :: push :: send, vine :: identifier>,
                   public eventemitter <events :: push :: receive, vine :: identifier>,
                   public eventemitter <events :: view :: join, vine :: identifier>,
                   public eventemitter <events :: view :: leave, vine :: identifier>
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

            static constexpr size_t alpha = 8;
            static constexpr size_t beta = 14;
            static constexpr size_t gamma = view :: size - (alpha + beta);

            static constexpr interval interval = 3_s;
        };

        // Using

        using eventemitter <events :: push :: send, vine :: identifier> :: on;
        using eventemitter <events :: push :: receive, vine :: identifier> :: on;
        using eventemitter <events :: view :: join, vine :: identifier> :: on;
        using eventemitter <events :: view :: leave, vine :: identifier> :: on;

    private:

        // Private using

        using eventemitter <events :: push :: send, vine :: identifier> :: emit;
        using eventemitter <events :: push :: receive, vine :: identifier> :: emit;
        using eventemitter <events :: view :: join, vine :: identifier> :: emit;
        using eventemitter <events :: view :: leave, vine :: identifier> :: emit;

    public:

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

        size_t _version;
        std :: vector <identifier> _pushslots;
        std :: array <pullslot, settings :: beta> _pullslots;

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

        // Methods

        void start();

    private:

        // Private methods

        void update_sample(const vine :: identifier &);
        void update_view(const std :: array <vine :: identifier, settings :: view :: size> &);
        optional <std :: array <vine :: identifier, settings :: view :: size>> next_view();

        promise <void> pull(vine :: identifier, size_t, size_t);
        promise <void> push(vine :: identifier);
        promise <void> serve(pool :: connection);

        promise <void> run();
    };
};

#endif
