#ifndef __drop__distributed__messenger__hpp
#define __drop__distributed__messenger__hpp

// Includes

#include "messenger.h"

namespace drop
{
    // arc

    // Constructors

    template <typename... types> messenger <types...> :: arc :: arc(const pool :: connection & connection, class crontab & crontab) : connection(connection), alive(true), lastsend(0), crontab(crontab)
    {
        std :: cout << "Creating arc" << std :: endl; // REMOVE ME
    }

    // messenger

    // Constructors

    template <typename... types> messenger <types...> :: messenger(const pool :: connection & connection, crontab & crontab) : _arc(std :: make_shared <arc> (connection, crontab))
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
                std :: cout << "[send] Out of the pop" << std :: endl;

                arc->mutex.lock();
                bool alive = arc->alive;
                arc->lastsend = now;
                arc->mutex.unlock();

                if(!alive)
                {
                    std :: cout << "[send] Alive is false, returning" << std :: endl;
                    co_return;
                }

                std :: cout << "[send] Sending something" << std :: endl;
                co_await arc->connection.send(message);
            }
        }
        catch(...)
        {
            std :: cout << "[send] Exception" << std :: endl;

            arc->mutex.lock();
            arc->alive = false;
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
                std :: cout << "[receive] Out of the receive" << std :: endl;

                arc->mutex.lock();
                bool alive = arc->alive;
                arc->mutex.unlock();

                if(!alive)
                {
                    std :: cout << "[receive] Alive is false, returning" << std :: endl;
                    co_return;
                }

                std :: cout << "[receive] Received something" << std :: endl;

                if(message.empty())
                    std :: cout << "[receive] Just a keepalive" << std :: endl;
                else
                    std :: cout << "[receive] It's an actual message" << std :: endl; // Emit appropriate event
            }
        }
        catch(...)
        {
            std :: cout << "[receive] Exception" << std :: endl;

            arc->mutex.lock();
            arc->alive = false;
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
                    std :: cout << "[keepalive] Alive is false, waking send and returning" << std :: endl;
                    arc->pipe.push(variant <types...> ());
                    co_return;
                }

                if(timestamp(now) - lastsend > settings :: keepalive)
                {
                    std :: cout << "[keepalive] Sending keepalive" << std :: endl;
                    arc->pipe.push(variant <types...> ());
                }

                co_await arc->crontab.wait(settings :: keepalive);
            }
        }
        catch(...)
        {
            std :: cout << "[keepalive] Exception" << std :: endl;

            arc->mutex.lock();
            arc->alive = false;
            arc->mutex.unlock();
        }
    }
};

#endif
