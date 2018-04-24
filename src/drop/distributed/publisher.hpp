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

    // archive

    // Constructors

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: archive :: archive(const ttype & topic, const messenger <command, publication> & messenger) : _topic(&topic), _messenger(&messenger)
    {
    }

    template <typename ttype, typename ptype> publisher <ttype, ptype> :: archive :: archive(const ttype & topic, const messenger <command, publication> & messenger, bool & sent) : _topic(&topic), _messenger(&messenger), _sent(&sent)
    {
    }

    // Operators

    template <typename ttype, typename ptype> const typename publisher <ttype, ptype> :: archive & publisher <ttype, ptype> :: archive :: operator << (const ptype & payload) const
    {
        if((this->_sent) && !(**(this->_sent)))
        {
            this->_messenger->send(publication(*(this->_topic), payload));
            **sent = true;
        }
        else
            this->_messenger->send(publication(*(this->_topic), payload));
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

    // Methods

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: serve(const pool :: connection & connection)
    {
        this->_mutex.lock();
        id id = this->_nonce++;

        messenger <command, publication> messenger(connection, this->_crontab);

        messenger.template on <close> ([=]()
        {
            this->_mutex.lock();
            this->drop(id);
            this->_mutex.unlock();
        });

        messenger.template on <command> ([=](const command & command)
        {
            this->_mutex.lock();
            this->command(id, command);
            this->_mutex.unlock();
        });

        this->_sessions[id] = messenger;
        this->_sessions[id]->start();

        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: publish(const ttype & topic, const ptype & payload)
    {
        this->_mutex.lock();

        try
        {
            auto & set = this->_topics.at(topic);

            for(auto iterator = set->begin(); iterator != set->end();)
            {
                this->_sessions[iterator->id].send(publication(topic, payload));
                if(iterator->once)
                {
                    this->remove_from_session(*iterator);
                    iterator = set->erase(iterator);
                }
                else
                    iterator++;
            }

            if(set->empty())
                this->_topics.erase(topic);
        }
        catch(...)
        {
        }


        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> template <typename type, typename lambda, std :: enable_if_t <(std :: is_same <type, typename publisher <ttype, ptype> :: archive> :: value) && (eventemitter <typename publisher <ttype, ptype> :: archive, typename publisher <ttype, ptype> :: archive> :: constraints :: template callback <lambda> ())> *> void publisher <ttype, ptype> :: on(const lambda & handler)
    {
        this->_emitter.template on <archive> (handler);
    }

    // Private methods

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: command(const id & id, const command & command)
    {
        const messenger <command, publication> & messenger = this->_sessions[id];

        switch(command.type)
        {
            case commands :: subscribe:
            {
                this->_emitter.template emit <archive> (archive(command.topic, messenger));

                this->add({command.topic, id, false});
                break;
            }
            case commands :: unsubscribe:
            {
                this->remove({command.topic, id, false});
                break;
            }
            case commands :: once:
            {
                bool sent = false;
                this->_emitter.template emit <archive> (archive(command.topic, messenger, sent));

                if(!sent)
                    this->add({command.topic, id, true});
                break;
            }
        }
    }

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

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: drop(const id & id)
    {
        this->clear(id);
        this->_sessions.erase(id);
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
