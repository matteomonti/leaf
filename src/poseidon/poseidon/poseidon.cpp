// Includes

#include "poseidon.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _gossiper(this->_signer.publickey(), crontab), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }
};
