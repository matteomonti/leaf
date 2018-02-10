#ifndef __drop__bytewise__bytewise__hpp
#define __drop__bytewise__bytewise__hpp

// Includes

#include "bytewise.h"
#include "endianess.hpp"

namespace drop
{
    // constraints

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: acceptor()
    {
        if constexpr (std :: is_array <atype> :: value)
            return acceptor <std :: remove_all_extents_t <atype>, vtype> ();
        else
            return traits :: can_accept <atype, vtype> :: value || std :: is_integral <atype> :: value;
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: visitor()
    {
        return traits :: can_update <vtype> :: value;
    }

    // visitor

    // Constructors

    template <typename vtype> bytewise :: visitor <vtype> :: visitor(vtype & visitor) : _visitor(visitor)
    {
    }

    // Operators

    template <typename vtype> template <typename atype, std :: enable_if_t <bytewise :: constraints :: acceptor <atype, vtype> ()> *> inline bytewise :: visitor <vtype> & bytewise :: visitor <vtype> :: operator << (const atype & acceptor)
    {
        if constexpr (std :: is_array <atype> :: value)
            for(size_t i = 0; i < std :: extent <atype, std :: rank <atype> :: value - 1> :: value; i++)
                (*this) << acceptor[i];
        else if constexpr (traits :: can_accept <atype, vtype> :: value)
            acceptor.accept(*this);
        else if constexpr (std :: is_integral <atype> :: value)
        {
            atype translated = endianess :: translate(acceptor);
            this->_visitor.update((const uint8_t *) &translated, sizeof(atype));
        }
        return *this;
    }
};

#endif
