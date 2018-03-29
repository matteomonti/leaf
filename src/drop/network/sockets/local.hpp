#ifndef __drop__network__sockets__local__hpp
#define __drop__network__sockets__local__hpp

// Includes

#include "local.h"
#include "traits.hpp"

namespace drop :: sockets
{
    // Methods

    template <typename stype, std :: enable_if_t <constraints :: streamer <stype> ()> *> bool local :: receive(stype & streamer)
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
