// Forward declarations

namespace drop
{
    template <typename> class pipe;
};

#if !defined(__forward__) && !defined(__drop__async__pipe__h)
#define __drop__async__pipe__h

// Libraries

#include <mutex>
#include <deque>

// Includes

#include "promise.hpp"
#include "drop/data/optional.hpp"

namespace drop
{
    template <typename type> class pipe
    {
        // Members

        optional <drop :: promise <type>> _promise;
        std :: deque <type> _pipe;
        std :: mutex _mutex;

    public:

        // Methods

        void push(const type &);
        promise <type> pop();
    };
};

#endif
