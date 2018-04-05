// Forward declarations

namespace poseidon
{
    template <size_t, size_t> class pullpool;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__pullpool__h)
#define __poseidon__brahms__pullpool__h

// Libraries

#include <sodium.h>
#include <array>

// Forward includes

#define __forward__
#include "brahms.h"
#include "view.h"
#undef __forward__

// Includes

#include "vine/network/dial.h"
#include "drop/data/optional.hpp"

namespace poseidon
{
    using namespace vine;
    using namespace drop;

    template <size_t psize, size_t vsize> class pullpool
    {
        // Friends

        friend class brahms;
        template <size_t> friend class view;

        // Members

        identifier _identifier;

        std :: array <optional <std :: array <identifier, vsize>>, psize> _slots;
        size_t _version;

        // Constructors

        pullpool(const identifier &);

        // Methods

        size_t init();
        void set(const size_t &, const size_t &, const std :: array <identifier, vsize> &);

        const identifier & random();

        // Casting

        operator bool ();
    };
};

#endif
