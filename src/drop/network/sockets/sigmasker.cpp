// Includes

#include "sigmasker.h"

namespace drop :: sockets
{
    // Constructors

    sigmasker :: sigmasker()
    {
        signal(SIGPIPE, SIG_IGN);
    }

    sigmasker sigmasker :: masker;
};
