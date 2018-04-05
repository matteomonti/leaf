// Forward declarations

namespace poseidon
{
    template <size_t> class view;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__view__h)
#define __poseidon__brahms__view__h

// Libraries

#include <sodium.h>
#include <array>

// Forward includes

#define __forward__
#include "brahms.h"
#undef __forward__

// Includes

#include "pushpool.h"
#include "pullpool.h"
#include "sample.h"
#include "vine/network/dial.h"

namespace poseidon
{
    using namespace vine;

    template <size_t size> class view : public std :: array <identifier, size>
    {
        // Friends

        friend class brahms;

        // Constructors

        view(const std :: array <identifier, size> &);

        // Methods

        template <typename lambda> void distinct(const lambda &) const;
        template <typename alambda, typename rlambda> void diff(const view &, const alambda &, const rlambda &) const;

        const identifier & random() const;

        // Private methods

        void increment(size_t &) const;

        // Static methods

        template <size_t alpha, size_t beta, size_t gamma, size_t pushsize, size_t pullpsize, size_t pullvsize, size_t samplesize> static view from(pushpool <pushsize> &, pullpool <pullpsize, pullvsize> &, sample <samplesize> &);
    };
};

#endif
