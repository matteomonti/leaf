#ifndef __drop__bytewise__bytewise__hpp
#define __drop__bytewise__bytewise__hpp

// Includes

#include "bytewise.h"
#include "endianess.hpp"
#include "drop/data/varint.hpp"

namespace drop
{
    // constraints

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: readable()
    {
        if constexpr (std :: is_array <atype> :: value)
            return readable <std :: remove_all_extents_t <atype>, vtype> ();
        else if constexpr (traits :: is_vector <atype> :: value)
            return readable <typename traits :: is_vector <atype> :: type, vtype> ();
        else
            return traits :: can_accept_reader <atype, vtype> :: value || std :: is_integral <atype> :: value;
    }

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: writable()
    {
        if constexpr (std :: is_array <atype> :: value)
            return writable <std :: remove_all_extents_t <atype>, vtype> ();
        else if constexpr (traits :: is_vector <atype> :: value)
            return writable <typename traits :: is_vector <atype> :: type, vtype> ();
        else
            return traits :: can_accept_writer <atype, vtype> :: value || std :: is_integral <atype> :: value;
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: reader()
    {
        return traits :: can_update <vtype> :: value;
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: writer()
    {
        return traits :: can_pop <vtype> :: value;
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

    template <typename vtype> template <typename atype, std :: enable_if_t <bytewise :: constraints :: readable <atype, vtype> ()> *> inline bytewise :: reader <vtype> & bytewise :: reader <vtype> :: operator << (const atype & acceptor)
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
        else if constexpr (traits :: can_accept_reader <atype, vtype> :: value)
            acceptor.accept(*this);
        else if constexpr (std :: is_integral <atype> :: value)
        {
            atype translated = endianess :: translate(acceptor);
            this->_reader.update((const uint8_t *) &translated, sizeof(atype));
        }
        return *this;
    }

    // writer

    // Constructors

    template <typename vtype> bytewise :: writer <vtype> :: writer(vtype & writer) : _writer(writer)
    {
    }

    // Methods

    template <typename vtype> inline const uint8_t * bytewise :: writer <vtype> :: pop(const size_t & size)
    {
        return this->_writer.pop(size);
    }

    // Operators

    template <typename vtype> template <typename atype, std :: enable_if_t <bytewise :: constraints :: writable <atype, vtype> ()> *> inline bytewise :: writer <vtype> & bytewise :: writer <vtype> :: operator >> (atype & acceptor)
    {
        if constexpr (std :: is_array <atype> :: value)
            for(size_t i = 0; i < std :: extent <atype, std :: rank <atype> :: value - 1> :: value; i++)
                (*this) >> acceptor[i];
        else if constexpr (traits :: is_vector <atype> :: value)
        {
            varint size;
            (*this) >> size;

            acceptor.resize(size);
            for(size_t i = 0; i < size; i++)
                (*this) >> acceptor[i];
        }
        else if constexpr (traits :: can_accept_writer <atype, vtype> :: value)
            acceptor.accept(*this);
        else if constexpr (std :: is_integral <atype> :: value)
        {
            const uint8_t * value = this->_writer.pop(sizeof(atype));
            acceptor = endianess :: translate(*((atype *) value));
        }
        return *this;
    }

    // bytewise

    // Static methods

    template <typename vtype, typename... atypes, std :: enable_if_t <bytewise :: constraints :: reader <vtype> () && (... && (bytewise :: constraints :: readable <atypes, vtype> ()))> *> inline void bytewise :: read(vtype & wrappee, const atypes & ... acceptors)
    {
        reader <vtype> wrapper(wrappee);
        (wrapper << ... << acceptors);
    }

    template <typename vtype, typename atype, typename... atypes, std :: enable_if_t <bytewise :: constraints :: writer <vtype> () && bytewise :: constraints :: writable <atype, vtype> () && (... && (bytewise :: constraints :: writable <atypes, vtype> ()))> *> inline void bytewise :: write(vtype & wrappee, atype & acceptor, atypes & ... acceptors)
    {
        writer <vtype> wrapper(wrappee);
        wrapper >> acceptor;

        if constexpr (sizeof...(acceptors) > 0)
            write(wrappee, acceptors...);

        // (wrapper >> ... >> acceptors);
    }
};

#endif
