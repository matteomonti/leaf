#ifndef __drop__distributed__publisher__hpp
#define __drop__distributed__publisher__hpp

// Includes

#include "publisher.h"

namespace drop
{
    // Constraints

    template <typename ttype, typename ptype> constexpr bool publisher <ttype, ptype> :: constraints :: valid()
    {
        return bytewise :: constraints :: serializable <ttype> () && bytewise :: constraints :: deserializable <ttype> () && bytewise :: constraints :: serializable <ptype> () && bytewise :: constraints :: deserializable <ptype> ();
    }

    // command

    // Constructors

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: command :: command(const ttype & topic, const typename commands :: type & type) : topic(topic), type(type)
    {
    }

    // Methods

    template <typename ttype, typename ptype> template <typename vtype> void publisher <ttype, ptype> :: command :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->topic);
        reader << (this->type);
    }

    template <typename ttype, typename ptype> template <typename vtype> void publisher <ttype, ptype> :: command :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->topic);
        writer >> (this->type);
    }

    // publication

    // Constructor

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: publication :: publication(const ttype & topic, const ptype & payload) : topic(topic), payload(payload)
    {
    }

    // Methods

    template <typename ttype, typename ptype> template <typename vtype> void publisher <ttype, ptype> :: publication :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->topic);
        reader << (this->payload);
    }

    template <typename ttype, typename ptype> template <typename vtype> void publisher <ttype, ptype> :: publication :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->topic);
        writer >> (this->payload);
    }

    // subscription

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: subscription :: subscription(const publisher :: id & id, const bool & once) : id(id), once(once)
    {
    }

    // publisher

    // Constructors

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: publisher(crontab & crontab) : _nonce(1), _crontab(crontab)
    {
    }
};

#endif
