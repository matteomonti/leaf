#ifndef __drop__bytewise__bytewise__hpp
#define __drop__bytewise__bytewise__hpp

// Includes

#include "bytewise.h"
#include "endianess.hpp"
#include "drop/utils/sfinae.hpp"
#include "drop/data/varint.hpp"
#include "drop/data/buffer.hpp"
#include "drop/utils/visit.hpp"

namespace drop
{
    // Traits

    template <typename atype, typename vtype> constexpr bool bytewise :: traits :: can_accept_reader()
    {
        return sfinae :: is_valid([](auto && acceptor) -> decltype(acceptor.accept(std :: declval <reader <vtype> &> ())) {}).template satisfied <const atype> ();
    }

    template <typename atype, typename vtype> constexpr bool bytewise :: traits :: can_accept_writer()
    {
        return sfinae :: is_valid([](auto && acceptor) -> decltype(acceptor.accept(std :: declval <writer <vtype> &> ())) {}).template satisfied <atype> ();
    }

    template <typename utype> constexpr bool bytewise :: traits :: can_update()
    {
        return sfinae :: is_valid([](auto && object) -> decltype(object.update(std :: declval <const uint8_t *> (), std :: declval <const size_t> ())) {}).template satisfied <utype> ();
    }

    template <typename utype> constexpr bool bytewise :: traits :: can_pop()
    {
        return sfinae :: returns <const uint8_t *> ([](auto && object) -> decltype(object.pop(std :: declval <const size_t> ())) {}).template satisfied <utype> ();
    }

    // Constraints

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: readable()
    {
        if constexpr (std :: is_array <atype> :: value)
            return readable <std :: remove_all_extents_t <atype>, vtype> ();
        else if constexpr (traits :: is_vector <atype> :: value)
            return readable <typename traits :: is_vector <atype> :: type, vtype> ();
        else if constexpr (traits :: is_std_array <atype> :: value)
            return readable <typename traits :: is_std_array <atype> :: type, vtype> ();
        else
            return traits :: can_accept_reader <atype, vtype> () || std :: is_integral <atype> :: value;
    }

    template <typename atype, typename vtype> constexpr bool bytewise :: constraints :: writable()
    {
        if constexpr (std :: is_array <atype> :: value)
            return writable <std :: remove_all_extents_t <atype>, vtype> ();
        else if constexpr (traits :: is_vector <atype> :: value)
            return writable <typename traits :: is_vector <atype> :: type, vtype> ();
        else if constexpr (traits :: is_std_array <atype> :: value)
            return writable <typename traits :: is_std_array <atype> :: type, vtype> ();
        else
            return traits :: can_accept_writer <atype, vtype> () || std :: is_integral <atype> :: value;
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: reader()
    {
        return traits :: can_update <vtype> ();
    }

    template <typename vtype> constexpr bool bytewise :: constraints :: writer()
    {
        return traits :: can_pop <vtype> ();
    }

    template <typename atype> constexpr bool bytewise :: constraints :: serializable()
    {
        return readable <atype, sizer> () && readable <atype, rstream> ();
    }

    template <typename atype> constexpr bool bytewise :: constraints :: deserializable()
    {
        return std :: is_default_constructible <atype> :: value && writable <atype, wstream> ();
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
        else if constexpr (traits :: is_std_array <atype> :: value)
            for(size_t i = 0; i < acceptor.size(); i++)
                (*this) << acceptor[i];
        else if constexpr (traits :: can_accept_reader <atype, vtype> ())
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
        else if constexpr (traits :: is_std_array <atype> :: value)
            for(size_t i = 0; i < acceptor.size(); i++)
                (*this) >> acceptor[i];
        else if constexpr (traits :: can_accept_writer <atype, vtype> ())
            acceptor.accept(*this);
        else if constexpr (std :: is_integral <atype> :: value)
        {
            const uint8_t * value = this->_writer.pop(sizeof(atype));
            acceptor = endianess :: translate(*((atype *) value));
        }
        return *this;
    }

    // sizer

    // Constructors

    inline bytewise :: sizer :: sizer() : size(0)
    {
    }

    // Methods

    inline void bytewise :: sizer :: update(const uint8_t *, const size_t & size)
    {
        this->size += size;
    }

    // rstream

    // Constructors

    inline bytewise :: rstream :: rstream(buffer & bytes) : bytes(bytes), cursor(0)
    {
    }

    // Methods

    inline void bytewise :: rstream :: update(const uint8_t * bytes, const size_t & size)
    {
        memcpy((char *)(this->bytes) + this->cursor, bytes, size);
        this->cursor += size;
    }

    // wstream

    // Constructors

    inline bytewise :: wstream :: wstream(const buffer & bytes) : bytes(bytes), cursor(0)
    {
    }

    // Methods

    inline const uint8_t * bytewise :: wstream :: pop(const size_t & size)
    {
        if(this->cursor + size > this->bytes.size())
            throw "Error!"; // TODO: throw a more meaningful error

        const uint8_t * ret = ((const uint8_t *)(this->bytes) + this->cursor);
        this->cursor += size;

        return ret;
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

    template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> *> inline buffer bytewise :: serialize(const atypes & ... acceptors)
    {
        sizer sizer;
        read(sizer, acceptors...);

        buffer bytes;
        bytes.alloc(sizer.size);

        rstream rstream(bytes);
        read(rstream, acceptors...);

        return bytes;
    }

    template <typename atype, std :: enable_if_t <bytewise :: constraints :: deserializable <atype> ()> *> inline atype bytewise :: deserialize(const buffer & bytes)
    {
        wstream wstream(bytes);

        atype acceptor;
        write(wstream, acceptor);

        return acceptor;
    }

    template <typename... atypes, std :: enable_if_t <(sizeof...(atypes) > 1) && (... && (bytewise :: constraints :: deserializable <atypes> ()))> *> inline std :: tuple <atypes...> bytewise :: deserialize(const buffer & bytes)
    {
        wstream wstream(bytes);

        std :: tuple <atypes...> acceptors;
        visit :: tuple(acceptors, [&](auto & ... acceptors)
        {
            write(wstream, acceptors...);
        });

        return acceptors;
    }

    template <typename atype, std :: enable_if_t <bytewise :: constraints :: deserializable <atype> ()> *> inline atype bytewise :: deserialize(const uint8_t * bytes, const size_t & size)
    {
        return deserialize <atype> (buffer(buffer :: reference, const_cast <uint8_t *> (bytes), size));
    }

    template <typename... atypes, std :: enable_if_t <(sizeof...(atypes) > 1) && (... && (bytewise :: constraints :: deserializable <atypes> ()))> *> inline std :: tuple <atypes...> bytewise :: deserialize(const uint8_t * bytes, const size_t & size)
    {
        return deserialize <atypes...> (buffer(buffer :: reference, const_cast <uint8_t *> (bytes), size));
    }
};

#endif
