#ifndef __drop__distributed__messenger__hpp
#define __drop__distributed__messenger__hpp

// Includes

#include "messenger.h"

namespace drop
{
    // Constraints

    template <typename... types> constexpr bool messenger <types...> :: constraints :: valid()
    {
        return (... && (bytewise :: constraints :: serializable <types> ())) && (... && (bytewise :: constraints :: deserializable <types> ()));
    }

    // Exceptions

    template <typename... types> const char * messenger <types...> :: exceptions :: messenger_deleted :: what() const throw()
    {
        return "Messenger deleted.";
    }

    // arc

    // Constructors

    template <typename... types> messenger <types...> :: arc :: arc(const pool :: connection & connection, class crontab & crontab) : connection(connection), alive(true), lastsend(0), crontab(crontab)
    {
    }

    // messenger

    // Constructors

    template <typename... types> messenger <types...> :: messenger()
    {
    }

    template <typename... types> messenger <types...> :: messenger(const pool :: connection & connection, crontab & crontab) : _arc(std :: make_shared <arc> (connection, crontab))
    {
    }

    // Methods

    template <typename... types> template <typename type, typename lambda, std :: enable_if_t <(... || (std :: is_same <type, types> :: value)) && (eventemitter <type, type> :: constraints :: template callback <lambda> ())> *> void messenger <types...> :: on(const lambda & handler) const
    {
        this->_arc->eventemitter <type, type> :: template on <type> (handler);
    }

    template <typename... types> template <typename type, std :: enable_if_t <(... || (std :: is_same <type, types> :: value))> *> void messenger <types...> :: send(const type & message) const
    {
        this->_arc->pipe.push(message);
    }

    template <typename... types> template <typename type, typename lambda, std :: enable_if_t <std :: is_same <type, close> :: value && eventemitter <close> :: constraints :: template callback <lambda> ()> *> void messenger <types...> :: on(const lambda & handler) const
    {
        this->_arc->eventemitter <close> :: template on <close> (handler);
    }

    template <typename... types> template <typename type, std :: enable_if_t <(std :: is_same <type, close> :: value) || (... || (std :: is_same <type, types> :: value))> *> void messenger <types...> :: clear() const
    {
        if constexpr (std :: is_same <type, close> :: value)
            this->_arc->eventemitter <close> :: clear();
        else
            this->_arc->eventemitter <type, type> :: clear();
    }

    template <typename... types> void messenger <types...> :: start() const
    {
        send(this->_arc);
        receive(this->_arc);
        keepalive(this->_arc);
    }

    // Static private methods

    template <typename... types> promise <void> messenger <types...> :: send(std :: shared_ptr <arc> arc)
    {
        try
        {
            while(true)
            {
                variant <types...> message = co_await arc->pipe.pop();

                // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock a " << &(arc->mutex) << std :: endl;
                arc->mutex.lock();
                bool alive = arc->alive;
                arc->lastsend = now;
                // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock a " << &(arc->mutex) << std :: endl;
                arc->mutex.unlock();

                if(!alive) break;

                co_await arc->connection.send(message);
            }
        }
        catch(...)
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock b " << &(arc->mutex) << std :: endl;
            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock b " << &(arc->mutex) << std :: endl;
            arc->mutex.unlock();
        }
    }

    template <typename... types> promise <void> messenger <types...> :: receive(std :: shared_ptr <arc> arc)
    {
        try
        {
            while(true)
            {
                variant <types...> message = co_await arc->connection.template receive <variant <types...>> ();

                // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock c " << &(arc->mutex) << std :: endl;
                arc->mutex.lock();
                bool alive = arc->alive;
                // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock c " << &(arc->mutex) << std :: endl;
                arc->mutex.unlock();

                if(!alive) break;

                if(!(message.empty()))
                {
                    message.visit([&](const auto & value)
                    {
                        typedef std :: remove_const_t <std :: remove_reference_t <decltype(value)>> base;
                        arc->eventemitter <base, base> :: template emit <base> (value);
                    });
                }
            }
        }
        catch(...)
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock d " << &(arc->mutex) << std :: endl;
            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock d " << &(arc->mutex) << std :: endl;
            arc->mutex.unlock();
        }
    }

    template <typename... types> promise <void> messenger <types...> :: keepalive(std :: shared_ptr <arc> arc)
    {
        try
        {
            while(true)
            {
                // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock e " << &(arc->mutex) << std :: endl;
                arc->mutex.lock();
                bool alive = arc->alive;
                timestamp lastsend = arc->lastsend;
                // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock e " << &(arc->mutex) << std :: endl;
                arc->mutex.unlock();

                if(!alive) break;

                if(arc.use_count() <= 3)
                    throw (class exceptions :: messenger_deleted){};

                if(timestamp(now) - lastsend > settings :: keepalive)
                    arc->pipe.push(variant <types...> ());

                co_await arc->crontab.wait(settings :: keepalive);
            }
        }
        catch(...)
        {
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger lock f " << &(arc->mutex) << std :: endl;
            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
            // std :: cout << "(" << std::this_thread::get_id() << ") messenger unlock f " << &(arc->mutex) << std :: endl;
            arc->mutex.unlock();
        }

        arc->pipe.push(variant <types...> ());
    }
};

#endif
