#ifndef __drop__distributed__messenger__hpp
#define __drop__distributed__messenger__hpp

// Includes

#include "messenger.h"

namespace drop
{
    // arc

    // Constructors

    template <typename... types> messenger <types...> :: arc :: arc(const pool :: connection & connection, class crontab & crontab, const char * prefix) : connection(connection), alive(true), lastsend(0), crontab(crontab), prefix(prefix)
    {
        std :: cout << prefix << "Creating arc" << std :: endl; // REMOVE ME
    }

    // messenger

    // Constructors

    template <typename... types> messenger <types...> :: messenger(const pool :: connection & connection, crontab & crontab, const char * prefix) : _arc(std :: make_shared <arc> (connection, crontab, prefix))
    {
        send(this->_arc);
        receive(this->_arc);
        keepalive(this->_arc);
    }

    // Methods

    template <typename... types> template <typename type, typename lambda, std :: enable_if_t <(... || (std :: is_same <type, types> :: value)) && (eventemitter <type, type> :: constraints :: template callback <lambda> ())> *> void messenger <types...> :: on(const lambda & handler)
    {
        this->_arc->eventemitter <type, type> :: template on <type> (handler);
    }

    template <typename... types> template <typename type, std :: enable_if_t <(... || (std :: is_same <type, types> :: value))> *> void messenger <types...> :: send(const type & message)
    {
        this->_arc->pipe.push(message);
    }

    template <typename... types> template <typename type, typename lambda, std :: enable_if_t <std :: is_same <type, close> :: value && eventemitter <close> :: constraints :: template callback <lambda> ()> *> void messenger <types...> :: on(const lambda & handler)
    {
        this->_arc->eventemitter <close> :: template on <close> (handler);
    }

    // Static private methods

    template <typename... types> promise <void> messenger <types...> :: send(std :: shared_ptr <arc> arc)
    {
        try
        {
            while(true)
            {
                variant <types...> message = co_await arc->pipe.pop();
                std :: cout << arc->prefix << "[send] Out of the pop" << std :: endl;

                arc->mutex.lock();
                bool alive = arc->alive;
                arc->lastsend = now;
                arc->mutex.unlock();

                if(!alive)
                {
                    std :: cout << arc->prefix << "[send] Alive is false, breaking" << std :: endl;
                    break;
                }

                std :: cout << arc->prefix << "[send] Sending something" << std :: endl;
                co_await arc->connection.send(message);
            }
        }
        catch(...)
        {
            std :: cout << arc->prefix << "[send] Exception" << std :: endl;

            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
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
                std :: cout << arc->prefix << "[receive] Out of the receive" << std :: endl;

                arc->mutex.lock();
                bool alive = arc->alive;
                arc->mutex.unlock();

                if(!alive)
                {
                    std :: cout << arc->prefix << "[receive] Alive is false, breaking" << std :: endl;
                    break;
                }

                std :: cout << arc->prefix << "[receive] Received something" << std :: endl;

                if(message.empty())
                    std :: cout << arc->prefix << "[receive] Just a keepalive" << std :: endl;
                else
                {
                    std :: cout << arc->prefix << "[receive] It's an actual message" << std :: endl;
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
            std :: cout << arc->prefix << "[receive] Exception" << std :: endl;

            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
            arc->mutex.unlock();
        }
    }

    template <typename... types> promise <void> messenger <types...> :: keepalive(std :: shared_ptr <arc> arc)
    {
        try
        {
            while(true)
            {
                arc->mutex.lock();
                bool alive = arc->alive;
                timestamp lastsend = arc->lastsend;
                arc->mutex.unlock();

                if(!alive)
                {
                    std :: cout << arc->prefix << "[keepalive] Alive is false, breaking" << std :: endl;
                    break;
                }

                if(arc.use_count() <= 3)
                {
                    std :: cout << arc->prefix << "[keepalive] Detected references being under three, throwing" << std :: endl;
                    throw "messenger deleted";
                }

                if(timestamp(now) - lastsend > settings :: keepalive)
                {
                    std :: cout << arc->prefix << "[keepalive] Sending keepalive" << std :: endl;
                    arc->pipe.push(variant <types...> ());
                }
                else
                    std :: cout << arc->prefix << "[keepalive] Last send was recent, no need to keepalive" << std :: endl;

                co_await arc->crontab.wait(settings :: keepalive);
            }
        }
        catch(...)
        {
            std :: cout << arc->prefix << "[keepalive] Exception" << std :: endl;

            arc->mutex.lock();
            if(arc->alive)
            {
                arc->eventemitter <close> :: emit <close> ();
                arc->alive = false;
            }
            arc->mutex.unlock();
        }

        std :: cout << arc->prefix << "[keepalive] Before exiting, pushing to make sure send quits as well." << std :: endl;
        arc->pipe.push(variant <types...> ());
    }
};

#endif
