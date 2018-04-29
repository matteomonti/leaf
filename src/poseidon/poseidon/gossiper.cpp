// Includes

#include "gossiper.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    gossiper :: gossiper(brahms & brahms, dialer & dialer, crontab & crontab) : _brahms(brahms), _gossiper(crontab), _dialer(dialer), _crontab(crontab)
    {
    }

    // Private methods

    promise <void> gossiper :: ban()
    {
        while(true)
        {
            co_await this->_crontab.wait(settings :: gossiper :: intervals :: ban);

            this->_mutex.lock();

            for(auto iterator = this->_scores.begin(); iterator != this->_scores.end();)
            {
                iterator->second--;
                if(iterator->second <= settings :: gossiper :: thresholds :: ban)
                {
                    this->_brahms.ban(iterator->first);
                    iterator = this->_scores.erase(iterator);
                }
                else
                    iterator++;
            }

            this->_mutex.unlock();
        }
    }
};
