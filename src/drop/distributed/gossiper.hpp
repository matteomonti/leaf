#ifndef __drop__distributed__gossiper__hpp
#define __drop__distributed__gossiper__hpp

// Includes

#include "gossiper.h"

namespace drop
{
    // Exceptions

    template <typename type> const char * gossiper <type> :: exceptions :: buffer_overflow :: what() const throw()
    {
        return "Buffer overflow. Rejecting connection to allow for merge into the syncset.";
    }

    // handle

    // Private constructors

    template <typename type> gossiper <type> :: handle :: handle(const id & id, const promise <void> & promise, gossiper <type> & gossiper) : drop :: promise <void> (promise), _id(id), _gossiper(&gossiper)
    {
    }

    // Getters

    template <typename type> bool gossiper <type> :: handle :: alive() const
    {
        bool alive;

        this->_gossiper->_mutex.lock();
        try
        {
            this->_gossiper->_messengers.at(this->_id);
            alive = true;
        }
        catch(...)
        {
            alive = false;
        }
        this->_gossiper->_mutex.unlock();

        return alive;
    }

    // Methods

    template <typename type> void gossiper <type> :: handle :: close() const
    {
        this->_gossiper->_mutex.lock();
        this->_gossiper->drop(this->_id);
        this->_gossiper->_mutex.unlock();
    }

    // Casting

    template <typename type> gossiper <type> :: handle :: operator const id & () const
    {
        return this->_id;
    }

    // gossiper

    // Constructors

    template <typename type> gossiper <type> :: gossiper(crontab & crontab) : _locks(0), _nonce(1), _crontab(crontab)
    {
    }

    // Methods

    template <typename type> void gossiper <type> :: publish(const type & element)
    {
        this->_mutex.lock();
        this->gossip(0, element);
        this->_mutex.unlock();
    }

    template <typename type> typename gossiper <type> :: handle gossiper <type> :: serve(const pool :: connection & connection, const bool & tiebreaker)
    {
        this->_mutex.lock();

        id id = this->_nonce++;
        promise <void> promise;

        this->serve(id, connection, tiebreaker, promise);

        this->_mutex.unlock();

        return handle(id, promise, *this);
    }

    // Private methods

    template <typename type> void gossiper <type> :: lock()
    {
        if(this->_addbuffer.size() > settings :: buffer :: failsafe)
            throw (class exceptions :: buffer_overflow){};
        else
            this->_locks++;
    }

    template <typename type> void gossiper <type> :: unlock()
    {
        this->_locks--;
        if(this->_locks == 0)
            this->merge();
    }

    template <typename type> promise <void> gossiper <type> :: serve(id id, pool :: connection connection, bool tiebreaker, promise <void> promise)
    {
        try
        {
            this->_mutex.lock();
            this->lock();
            this->_mutex.unlock();
        }
        catch(...)
        {
            this->_mutex.unlock();
            co_return;
        }

        try
        {
            co_await this->sync(id, connection, tiebreaker);
        }
        catch(...)
        {
            this->_mutex.lock();
            this->unlock();
            this->_mutex.unlock();

            co_return;
        }

        this->_mutex.lock();

        drop :: messenger <type> messenger(connection, this->_crontab);

        messenger.template on <close> ([=]()
        {
            this->_mutex.lock();
            this->drop(id);
            this->_mutex.unlock();
        });

        messenger.template on <type> ([=](const type & element)
        {
            this->_mutex.lock();
            this->gossip(id, element);
            this->_mutex.unlock();
        });

        for(const type & element : this->_addbuffer)
            messenger.send(element);

        this->_messengers[id] = (class messenger){.messenger = messenger, .promise = promise};
        this->_messengers[id].messenger.start();

        this->unlock();
        this->_mutex.unlock();
    }

    template <typename type> promise <void> gossiper <type> :: sync(id id, pool :: connection connection, bool tiebreaker)
    {
        if(tiebreaker)
            co_await connection.send(this->_syncset.sync().view);

        while(true)
        {
            auto view = co_await connection.receive <typename syncset <type> :: view> ();

            if(view.size() == 0)
                break;

            auto round = this->_syncset.sync(view);

            this->_mutex.lock();

            for(const type & element : round.add)
                this->gossip(id, element);

            this->_mutex.unlock();

            co_await connection.send(round.view);

            if(round.view.size() == 0)
                break;
        }
    }

    template <typename type> void gossiper <type> :: gossip(const id & id, const type & element)
    {
        if(!(this->_syncset.find(element)) && !(this->_addbuffer.count(element)))
            if(this->template emit <type> (id, element))
            {
                if(this->_locks == 0)
                {
                    this->_syncset.add(element);
                    this->merge();
                }
                else
                    this->_addbuffer.insert(element);

                for(const auto & messenger : this->_messengers)
                    messenger.second.messenger.send(element);
            }
    }

    template <typename type> void gossiper <type> :: merge()
    {
        for(const type & element : this->_addbuffer)
            this->_syncset.add(element);

        this->_addbuffer.clear();
    }

    template <typename type> void gossiper <type> :: drop(const id & id)
    {
        try
        {
            messenger & messenger = this->_messengers.at(id);
            messenger.promise.resolve();
            this->_messengers.erase(id);
        }
        catch(...)
        {
        }
    }
};

#endif
