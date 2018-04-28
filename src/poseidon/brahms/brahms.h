// Forward declarations

namespace poseidon
{
    class brahms;
};

#if !defined(__forward__) && !defined(__poseidon__poseidon__brahms__h)
#define __poseidon__poseidon__brahms__h

// Includes

#include "poseidon/settings.h"
#include "drop/network/pool.hpp"
#include "drop/chrono/crontab.h"
#include "drop/async/collector.hpp"

#include "vine/network/dial.h"
#include "sampler.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Typedefs

    typedef std :: array <identifier, settings :: view :: size> view;
    typedef std :: array <sampler, settings :: sample :: size> sample;

    // Classes

    class brahms
    {
    private:

        // Members

        view _view;
        sample _sample;

        pool & _pool;
        crontab & _crontab;

        std :: mutex _mutex;
    };
};

#endif
