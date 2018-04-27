// Forward declarations

namespace poseidon
{
    namespace staticsample
    {
        class master;
    };
};

#if !defined(__forward__) && !defined(__poseidon__benchmark__staticsample__master__h)
#define __poseidion__benchmark__staticsample__h

// Includes

#include "poseidon/benchmark/coordinator.h"
#include "vine/dialers/directory.hpp"

namespace poseidon :: staticsample
{
    using namespace drop;
    using namespace vine;

    class master
    {
        // Members

        coordinator _coordinator;
        dialers :: directory :: server _directory;

    public:

        // Constructors

        master(const uint16_t &, const uint16_t &, const size_t &);
    };
};

#endif
