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

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: add(const subscription & subscription)
    {
        try
        {
            this->_topics.at(subscription.topic);
        }
        catch(...)
        {
            std :: cout << "Creating topic" << std :: endl;
            this->_topics[subscription.topic] = std :: make_shared <std :: unordered_set <class subscription, shorthash>> ();
        }

        try
        {
            this->_sessions.at(subscription.id);
        }
        catch(...)
        {
            std :: cout << "Creating session" << std :: endl;
            this->_sessions[subscription.id] = std :: make_shared <std :: unordered_set <class subscription, shorthash>> ();
        }

        this->_topics[subscription.topic]->insert(subscription);
        this->_sessions[subscription.id]->insert(subscription);
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove(const subscription & subscription)
    {
        this->remove_from_topic(subscription);
        this->remove_from_session(subscription);
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: clear(const id & id)
    {
        try
        {
            auto & set = this->_sessions.at(id);

            for(const subscription & subscription : *set)
                this->remove_from_topic(subscription);

            this->_sessions.erase(id);

        }
        catch(...)
        {
        }
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove_from_topic(const subscription & subscription)
    {
        try
        {
            auto & set = this->_topics.at(subscription.topic);
            set->erase(subscription);

            if(set->empty())
            {
                std :: cout << "Deleting topic" << std :: endl;
                this->_topics.erase(subscription.topic);
            }
        }
        catch(...)
        {
        }
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove_from_session(const subscription & subscription)
    {
        try
        {
            auto & set = this->_sessions.at(subscription.id);
            set->erase(subscription);

            if(set->empty())
            {
                std :: cout << "Deleting session" << std :: endl;
                this->_sessions.erase(subscription.id);
            }
        }
        catch(...)
        {
        }
    }
};

#endif
