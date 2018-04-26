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

    // Private constructors

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
            **(this->_sent) = true;
        }
        else
            this->_messenger->send(publication(*(this->_topic), payload));

        return (*this);
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
        // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock a " << &(this->_mutex) << std :: endl;
        this->_mutex.lock();
        id id = this->_nonce++;

        messenger <command, publication> messenger(connection, this->_crontab);

        /*messenger.template on <close> ([=]()
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock b " << &(this->_mutex) << std :: endl;
            this->_mutex.lock();
            this->drop(id);
            // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock b " << &(this->_mutex) << std :: endl;
            this->_mutex.unlock();
        });*/ // REMOVE ME (comments, needs to be re-enabled)

        messenger.template on <command> ([=](const command & command)
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock c " << &(this->_mutex) << std :: endl;
            this->_mutex.lock();
            this->handle(id, command);
            // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock c " << &(this->_mutex) << std :: endl;
            this->_mutex.unlock();
        });

        this->_messengers[id] = messenger;
        this->_messengers[id].start();

        // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock a " << &(this->_mutex) << std :: endl;
        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: publish(const ttype & topic, const ptype & payload)
    {
        // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock d " << &(this->_mutex) << std :: endl;
        this->_mutex.lock();

        try
        {
            auto & set = this->_topics.at(topic);

            for(auto iterator = set->begin(); iterator != set->end();)
            {
                this->_messengers[iterator->id].send(publication(topic, payload));
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

        // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock d " << &(this->_mutex) << std :: endl;
        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> template <typename type, typename lambda, std :: enable_if_t <(std :: is_same <type, typename publisher <ttype, ptype> :: archive> :: value) && (eventemitter <typename publisher <ttype, ptype> :: archive, ttype, typename publisher <ttype, ptype> :: archive> :: constraints :: template callback <lambda> ())> *> void publisher <ttype, ptype> :: on(const lambda & handler)
    {
        this->_emitter.template on <archive> (handler);
    }

    // Private methods

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: handle(const id & id, const command & command)
    {
        const messenger <class command, publication> & messenger = this->_messengers[id];

        switch(command.type)
        {
            case commands :: subscribe:
            {
                this->_emitter.template emit <archive> (command.topic, archive(command.topic, messenger));
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
                this->_emitter.template emit <archive> (command.topic, archive(command.topic, messenger, sent));

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
            this->_topics[subscription.topic] = std :: make_shared <std :: unordered_set <class subscription, shorthash>> ();
        }

        try
        {
            this->_sessions.at(subscription.id);
        }
        catch(...)
        {
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
        this->_messengers.erase(id);
    }

    template <typename ttype, typename ptype> void publisher <ttype, ptype> :: remove_from_topic(const subscription & subscription)
    {
        try
        {
            auto & set = this->_topics.at(subscription.topic);
            set->erase(subscription);

            if(set->empty())
                this->_topics.erase(subscription.topic);
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
                this->_sessions.erase(subscription.id);
        }
        catch(...)
        {
        }
    }

    // subscription

    // Constructors

    template <typename ttype, typename ptype> subscriber <ttype, ptype> :: subscription :: subscription(const ttype & topic, const bool & once) : topic(topic), once(once)
    {
    }

    // Methods

    template <typename ttype, typename ptype> template <typename vtype> void subscriber <ttype, ptype> :: subscription :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->topic);
        reader << (this->once);
    }

    // Operators

    template <typename ttype, typename ptype> bool subscriber <ttype, ptype> :: subscription :: operator == (const subscription & rho) const
    {
        return (this->topic == rho.topic) && (this->once == rho.once);
    }

    // subscriber

    // Constructors

    template <typename ttype, typename ptype> subscriber <ttype, ptype> :: subscriber(const pool :: connection & connection, crontab & crontab) : _messenger(connection, crontab)
    {
        this->_messenger.template on <publication> ([=](const publication & publication)
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock e " << &(this->_mutex) << std :: endl;
            this->_mutex.lock();

            if(this->_subscriptions.count({publication.topic, false}))
                this->template emit <ptype> (publication.topic, publication.payload);

            if(this->_subscriptions.count({publication.topic, true}))
            {
                this->template emit <ptype> (publication.topic, publication.payload);
                this->_subscriptions.erase({publication.topic, true});
            }

            // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock e " << &(this->_mutex) << std :: endl;
            this->_mutex.unlock();
        });
    }

    // Methods

    template <typename ttype, typename ptype> void subscriber <ttype, ptype> :: subscribe(const ttype & topic)
    {
        // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock f " << &(this->_mutex) << std :: endl;
        this->_mutex.lock();

        subscription subscription(topic, false);

        if(!(this->_subscriptions.count(subscription)))
        {
            this->_subscriptions.insert(subscription);
            this->_messenger.send(command(topic, commands :: subscribe));
        }

        // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock f " << &(this->_mutex) << std :: endl;
        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> void subscriber <ttype, ptype> :: unsubscribe(const ttype & topic)
    {
        // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock g " << &(this->_mutex) << std :: endl;
        this->_mutex.lock();

        subscription subscription(topic, false);

        if(this->_subscriptions.count(subscription))
        {
            this->_messenger.send(command(topic, commands :: unsubscribe));
            this->_subscriptions.erase(subscription);
        }

        // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock g " << &(this->_mutex) << std :: endl;
        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> void subscriber <ttype, ptype> :: once(const ttype & topic)
    {
        // std :: cout << "(" << std::this_thread::get_id() << ") publisher lock h " << &(this->_mutex) << std :: endl;
        this->_mutex.lock();

        subscription subscription(topic, true);

        if(!(this->_subscriptions.count(subscription)))
        {
            this->_subscriptions.insert(subscription);
            this->_messenger.send(command(topic, commands :: once));
        }

        // std :: cout << "(" << std::this_thread::get_id() << ") publisher unlock h " << &(this->_mutex) << std :: endl;
        this->_mutex.unlock();
    }

    template <typename ttype, typename ptype> template <typename type, typename lambda, std :: enable_if_t <std :: is_same <type, ptype> :: value && eventemitter <ptype, ttype, ptype> :: constraints :: template callback <lambda> ()> *> void subscriber <ttype, ptype> :: on(const lambda & handler)
    {
        this->eventemitter <ptype, ttype, ptype> :: template on <ptype> (handler);
    }

    template <typename ttype, typename ptype> template <typename type, typename lambda, std :: enable_if_t <std :: is_same <type, close> :: value && eventemitter <close> :: constraints :: template callback <lambda> ()> *> void subscriber <ttype, ptype> :: on(const lambda & handler)
    {
        this->_messenger.template on <close> (handler);
    }

    template <typename ttype, typename ptype> void subscriber <ttype, ptype> :: start()
    {
        this->_messenger.start();
    }
};

#endif
