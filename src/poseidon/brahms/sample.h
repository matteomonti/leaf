// Forward declarations

namespace poseidon
{
    template <size_t> class sample;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__sample__h)
#define __poseidon__brahms__sample__h

// Libraries

#include <array>
#include <algorithm>

// Forward includes

#define __forward__
#include "brahms.h"
#include "view.h"
#undef __forward__

// Includes

#include "sampler.h"

namespace poseidon
{
    using namespace vine;

    template <size_t size> class sample
    {
        // Friends

        friend class brahms;
        template <size_t> friend class view;

        // Members

        std :: array <sampler, size> _samplers;

        // Methods

        template <typename alambda, typename rlambda> void update(const identifier &, const alambda &, const rlambda &);

        const identifier & random();

        // Casting

        operator bool ();
    };
};

#endif
