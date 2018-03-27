// Forward declarations

namespace vine
{
    class dial;
};

#if !defined(__forward__) && !defined(__vine__network__dial__h)
#define __vine__network__dial__h

// Includes

#include "drop/network/connection.hpp"
#include "drop/crypto/signature.hpp"

namespace vine
{
    using namespace drop;

    typedef signature :: publickey identifier;

    class dial : public connection
    {
        // Members

        identifier _identifier;

    public:

        // Constructors

        dial(const identifier &, const connection &);

        // Getters

        const identifier & identifier() const;
    };
};

#endif
