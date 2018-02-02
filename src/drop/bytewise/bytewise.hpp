#ifndef __drop__bytewise__bytewise__hpp
#define __drop__bytewise__bytewise__hpp

// Includes

#include "bytewise.h"
#include "endianess.hpp"

namespace drop
{
    // visitor

    // Constructors

    template <typename vtype> bytewise :: visitor <vtype> :: visitor(vtype & visitor) : _visitor(visitor)
    {
    }

    // Operators

    template <typename vtype> template <typename ttype, std :: enable_if_t <std :: is_integral <ttype> :: value> *> inline bytewise :: visitor <vtype> & bytewise :: visitor <vtype> :: operator << (const ttype & target)
    {
        ttype translated = endianess :: translate(target);
        this->_visitor.update((const uint8_t *) &translated, sizeof(ttype));
        return *this;
    }

    template <typename vtype> template <typename atype, std :: enable_if_t <bytewise :: can_accept <atype, vtype> :: value> *> inline bytewise :: visitor <vtype> & bytewise :: visitor <vtype> :: operator << (const atype & acceptor)
    {
        acceptor.accept(*this);
        return *this;
    }
};

#endif
