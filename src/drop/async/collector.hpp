#ifndef __drop__async__collector__hpp
#define __drop__async__collector__hpp

// Includes

#include "collector.h"

namespace drop
{
    // Traits

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declstorage()
    {
        if constexpr (is_array <type> :: value)
        {
            typedef typename is_array <type> :: type ptype;
            static constexpr size_t size = is_array <type> :: size;

            if constexpr (std :: is_same <ptype, promise <void>> :: value)
                return std :: declval <std :: array <bool, size>> ();
            else
            {
                typedef typename is_promise <ptype> :: type ttype;
                return std :: declval <std :: array <optional <ttype>, size>> ();
            }
        }
        else
        {
            typedef typename is_promise <type> :: type ttype;
            if constexpr (std :: is_same <ttype, void> :: value)
                return std :: declval <bool> ();
            else
                return std :: declval <optional <ttype>> ();
        }
    }
};

#endif
