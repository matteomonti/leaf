#ifndef __drop__bytewise__bytewise__hpp
#define __drop__bytewise__bytewise__hpp

// Includes

#include "bytewise.h"
#include "endianess.hpp"
#include "drop/data/varint.hpp"

namespace drop
{
    // constraints

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: acceptor()
    {
        if constexpr (std :: is_array <atype> :: value)
            return acceptor <std :: remove_all_extents_t <atype>, vtype> ();
        else if constexpr (traits :: is_vector <atype> :: value)
            return acceptor <typename traits :: is_vector <atype> :: type, vtype> ();
        else
            return traits :: can_accept <atype, vtype> :: value || std :: is_integral <atype> :: value;
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: reader()
    {
        return traits :: can_update <vtype> :: value;
    }

    // reader

    // Constructors

    template <typename vtype> bytewise :: reader <vtype> :: reader(vtype & reader) : _reader(reader)
    {
    }

    // Methods

    template <typename vtype> inline bytewise :: reader <vtype> & bytewise :: reader <vtype> :: update(const uint8_t * bytes, const size_t & size)
    {
        this->_reader.update(bytes, size);
        return (*this);
    }

    // Operators

    template <typename vtype> template <typename atype, std :: enable_if_t <bytewise :: constraints :: acceptor <atype, vtype> ()> *> inline bytewise :: reader <vtype> & bytewise :: reader <vtype> :: operator << (const atype & acceptor)
    {
        if constexpr (std :: is_array <atype> :: value)
            for(size_t i = 0; i < std :: extent <atype, std :: rank <atype> :: value - 1> :: value; i++)
                (*this) << acceptor[i];
        else if constexpr (traits :: is_vector <atype> :: value)
        {
            (*this) << varint(acceptor.size());
            for(size_t i = 0; i < acceptor.size(); i++)
                (*this) << acceptor[i];
        }
        else if constexpr (traits :: can_accept <atype, vtype> :: value)
            acceptor.accept(*this);
        else if constexpr (std :: is_integral <atype> :: value)
        {
            atype translated = endianess :: translate(acceptor);
            this->_reader.update((const uint8_t *) &translated, sizeof(atype));
        }
        return *this;
    }

    // bytewise

    // Static methods

    template <typename vtype, typename... atypes, std :: enable_if_t <bytewise :: constraints :: reader <vtype> () && (... && (bytewise :: constraints :: acceptor <atypes, vtype> ()))> *> inline void bytewise :: visit(vtype & wrappee, const atypes & ... acceptors)
    {
        reader <vtype> wrapper(wrappee);
        (wrapper << ... << acceptors);
    }
};

#endif
