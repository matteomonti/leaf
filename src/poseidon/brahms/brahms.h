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

#include "view.hpp"
#include "sample.hpp"
#include "pullpool.hpp"
#include "pushpool.hpp"
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

        // Members

        signer _signer;

        view <settings :: view :: size> _view;
        sample <settings :: sample :: size> _sample;

        pushpool <settings :: alpha> _pushpool;
        pullpool <settings :: beta, settings :: view :: size> _pullpool;

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

        promise <void> pull(size_t, size_t, vine :: identifier);
        promise <void> push(vine :: identifier);
        promise <void> serve(vine :: identifier, pool :: connection);

        promise <void> run();
    };
};

#endif
