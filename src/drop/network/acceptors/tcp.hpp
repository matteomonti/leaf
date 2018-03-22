#ifndef __drop__network__acceptors__tcp__hpp
#define __drop__network__acceptors__tcp__hpp

// Includes

#include "tcp.h"
#include "drop/utils/sfinae.hpp"

namespace drop :: acceptors
{
    // callback

    // Constructors

    template <typename lambda> tcp :: async :: callback <lambda> :: callback(const lambda & callback) : _callback(callback)
    {
    }

    // Operators

    template <typename lambda> void tcp :: async :: callback <lambda> :: operator () (const connection & connection)
    {
        this->_callback(connection);
    }

    // async

    // Traits

    template <typename ctype> constexpr bool tcp :: async :: traits :: is_connection_callable()
    {
        return sfinae :: is_valid([](auto && callback) -> decltype(callback(std :: declval <const connection &> ())) {}).template satisfied <const ctype> ();
    }

    // Constraints

    template <typename ctype> constexpr bool tcp :: async :: constraints :: callback()
    {
        return traits :: is_connection_callable <ctype> ();
    }

    // Methods

    template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, connection> :: value) && (tcp :: async :: constraints :: callback <lambda> ())> *> void tcp :: async :: on(const lambda & handler)
    {
        size_t cursor = 0;
        while(this->_callbacks[cursor])
            cursor++;

        this->_callbacks[cursor] = new callback <lambda> {handler};
    }
};

#endif
