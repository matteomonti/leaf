// Forward declarations

namespace poseidon
{
    template <size_t> class pushpool;
};

#if !defined(__forward__) && !defined(__poseidon__brahms__pushpool__h)
#define __poseidon__brahms__pushpool__h

// Libraries

#include <sodium.h>
#include <vector>

// Forward includes

#define __forward__
#include "brahms.h"
#include "view.h"
#undef __forward__

// Includes

#include "vine/network/dial.h"

namespace poseidon
{
    using namespace vine;

    template <size_t size> class pushpool
    {
        // Friends

        friend class brahms;
        template <size_t> friend class view;
        
        // Members

        identifier _identifier;

        std :: vector <identifier> _slots;

        // Methods

        void init();
        void push(const identifier &);

        const identifier & random();

        // Casting

        operator bool ();
    };
};

#endif
