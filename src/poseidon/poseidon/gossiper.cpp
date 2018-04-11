// Includes

#include "gossiper.h"

namespace poseidon
{
    // Constructors

    gossiper :: gossiper(crontab & crontab, std :: ostream & log) : _crontab(crontab), log(log)
    {
    }

    // Methods

    promise <void> gossiper :: serve(pool :: connection connection)
    {
        log << "Serving a connection" << std :: endl;

        for(uint64_t i = 0; i < 10; i++)
        {
            co_await connection.send(i);
            uint64_t j = co_await connection.receive <uint64_t> ();

            if(j != i)
                throw "Wrong value!";

            co_await this->_crontab.wait(3_s);
        }
    }
};
