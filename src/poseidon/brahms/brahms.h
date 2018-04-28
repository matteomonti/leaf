// Forward declarations

namespace poseidon
{
    class brahms;
    struct push;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__brahms__h)
#define __poseidon__poseidon__brahms__h

// Includes

#include "poseidon/settings.h"
#include "drop/async/eventemitter.hpp"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "drop/async/collector.hpp"
#include "vine/network/dial.h"
#include "vine/network/multiplexer.hpp"
#include "vine/dialers/local.h"
#include "vine/dialers/directory.hpp"
#include "sampler.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Typedefs

    typedef std :: array <identifier, settings :: view :: size> view;
    typedef std :: array <sampler, settings :: sample :: size> sample;

    // Classes

    class brahms : public eventemitter <push, identifier>
    {
        // Typedefs

        typedef multiplexer <dialers :: local :: client, settings :: channels> dialer;

        // Members

        signer _signer;

        view _view;
        sample _sample;

        std :: vector <identifier> _pushbuffer;

        dialer & _dialer;
        pool & _pool;
        crontab & _crontab;

        std :: mutex _mutex;

    public:

        // Constructors

        brahms(const signer &, const view &, dialer &, pool &, crontab &);

        // Methods

        void start();

    private:

        // Private methods

        promise <void> push(identifier);
        void push();

        promise <view> pull(identifier);
        auto pull();

        promise <void> serve(pool :: connection, identifier);
        promise <void> run();

        void update_sample(const view &);
        void update_sample(const std :: vector <identifier> &);

        void reset_sampler(const size_t &);
    };

    // Events

    struct push {};
};

#endif
