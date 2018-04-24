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

    // Constructors

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

    // Constructors

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: subscription :: subscription(const ttype & topic, const publisher :: id & id, const bool & once) : topic(topic), id(id), once(once)
    {
    }

    // Methods

    template <typename ttype, typename ptype> template <typename vtype> void publisher <ttype, ptype> :: subscription :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->topic);
        reader << (this->id);
        reader << (this->once);
    }

    // Operators

    template <typename ttype, typename ptype> bool publisher <ttype, ptype> :: subscription :: operator == (const subscription & rho) const
    {
        return (this->topic == rho.topic) && (this->id == rho.id) && (this->once == rho.once);
    }

    // publisher

    // Constructors

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: publisher(crontab & crontab) : _nonce(1), _crontab(crontab)
    {
    }

    // Private methods

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: subscribe(const ttype & topic, const id & id, const bool & once)
    {
        try
        {
            this->_topics.at(topic);
        }
        catch(...)
        {
            std :: cout << "Creating topic" << std :: endl;
            this->_topics[topic] = std :: make_shared <std :: unordered_set <subscription, shorthash>> ();
        }

        try
        {
            this->_sessions.at(id);
        }
        catch(...)
        {
            std :: cout << "Creating session" << std :: endl;
            this->_sessions[id] = std :: make_shared <std :: unordered_set <subscription, shorthash>> ();
        }

        this->_topics[topic]->insert({topic, id, once});
        this->_sessions[id]->insert({topic, id, once});
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: unsubscribe(const ttype & topic, const id & id, const bool & once)
    {
        this->remove_from_topic(topic, id, once);
        this->remove_from_session(topic, id, once);
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove_from_topic(const ttype & topic, const id & id, const bool & once)
    {
        try
        {
            auto & set = this->_topics.at(topic);
            set->erase({topic, id, once});

            if(set->empty())
            {
                std :: cout << "Deleting topic" << std :: endl;
                this->_topics.erase(topic);
            }
        }
        catch(...)
        {
        }
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove_from_session(const ttype & topic, const id & id, const bool & once)
    {
        try
        {
            auto & set = this->_sessions.at(id);
            set->erase({topic, id, once});

            if(set->empty())
            {
                std :: cout << "Deleting session" << std :: endl;
                this->_sessions.erase(id);
            }
        }
        catch(...)
        {
        }
    }
};

#endif
