// Includes

#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    gossiper :: gossiper(brahms & brahms, dialer & dialer, crontab & crontab) : _brahms(brahms), _gossiper(crontab), _dialer(dialer)
    {
    }
};
