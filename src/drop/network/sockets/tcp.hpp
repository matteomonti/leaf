#ifndef __drop__network__sockets__tcp__hpp
#define __drop__network__sockets__tcp__hpp

// Includes

#include "tcp.h"
#include "traits.hpp"

namespace drop :: sockets
{
    // Methods

    template <typename stype, std :: enable_if_t <constraints :: streamer <stype> ()> *> bool tcp :: receive(stype & streamer)
    {
        while(streamer.pending())
        {
            uint8_t buffer[settings :: streamer :: buffer];

            size_t received = this->receive(buffer, (streamer.pending() < settings :: streamer :: buffer ? streamer.pending() : settings :: streamer :: buffer));

            streamer.update(buffer, received);

            if(!(this->available()))
                break;
        }

        return !(streamer.pending());
    }
};

#endif
