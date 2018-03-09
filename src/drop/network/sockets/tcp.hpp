#ifndef __drop__network__sockets__tcp__hpp
#define __drop__network__sockets__tcp__hpp

// Includes

#include "tcp.h"
#include "drop/utils/sfinae.hpp"

namespace drop :: sockets
{
    // Traits

    template <typename stype> constexpr bool tcp :: traits :: has_update_method()
    {
        return sfinae :: is_valid([](auto && streamer) -> decltype(streamer.update(std :: declval <const uint8_t *> (), std :: declval <const size_t &> ())) {}).template satisfied <stype> ();
    }

    template <typename stype> constexpr bool tcp :: traits :: has_pending_method()
    {
        return sfinae :: returns <size_t> ([](auto && streamer) -> decltype(streamer.pending()) {}).template satisfied <stype> ();
    }

    // Constraints

    template <typename stype> constexpr bool tcp :: constraints :: streamer()
    {
        return traits :: has_update_method <stype> () && traits :: has_pending_method <stype> ();
    }

    // Methods

    template <typename stype, std :: enable_if_t <tcp :: constraints :: streamer <stype> ()> *> bool tcp :: receive(stype & streamer)
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
