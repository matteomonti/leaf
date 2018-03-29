#ifndef __drop__network__sockets__traits__hpp
#define __drop__network__sockets__traits__hpp

// Includes

#include "traits.h"
#include "drop/utils/sfinae.hpp"

namespace drop :: sockets
{
    // Traits

    template <typename stype> constexpr bool traits :: has_update_method()
    {
        return sfinae :: is_valid([](auto && streamer) -> decltype(streamer.update(std :: declval <const uint8_t *> (), std :: declval <const size_t &> ())) {}).template satisfied <stype> ();
    }

    template <typename stype> constexpr bool traits :: has_pending_method()
    {
        return sfinae :: returns <size_t> ([](auto && streamer) -> decltype(streamer.pending()) {}).template satisfied <stype> ();
    }

    // Constraints

    template <typename stype> constexpr bool constraints :: streamer()
    {
        return traits :: has_update_method <stype> () && traits :: has_pending_method <stype> ();
    }

};

#endif
