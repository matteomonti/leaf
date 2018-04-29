// Includes

#include "pool.hpp"

namespace drop
{
    // Exceptions

    const char * pool :: exceptions :: event_error :: what() const throw()
    {
        return "Event error.";
    }

    // connection

    // Private constructors

    pool :: connection :: connection(pool & pool, const :: drop :: connection & connection) : _pool(pool), _arc(connection._arc)
    {
    }

    // Getters

    address pool :: connection :: remote() const
    {
        return this->_arc->remote();
    }

    bool pool :: connection :: authenticated() const
    {
        return this->_arc->authenticated();
    }

    // Methods

    promise <void> pool :: connection :: send(const buffer & message) const
    {
        return this->_pool.send(this->_arc, message);
    }

    promise <buffer> pool :: connection :: receive() const
    {
        return this->_pool.receive(this->_arc);
    }

    promise <void> pool :: connection :: authenticate(keyexchanger exchanger, class keyexchanger :: publickey remote) const
    {
        class secretbox :: nonce txnonce = secretbox :: nonce :: random();
        co_await this->send(txnonce);
        class secretbox :: nonce rxnonce = co_await this->receive <class secretbox :: nonce> ();

        keyexchanger :: sessionkey session = exchanger.exchange(remote);
        this->_arc->secretkeys(session.transmit(), txnonce, session.receive(), rxnonce);
    }

    // pool

    // Settings

    constexpr interval pool :: settings :: timeout;
    constexpr interval pool :: settings :: interval;

    // Constructors

    pool :: pool() : _alive(true)
    {
        this->_queue.add(this->_wakepipe, queue :: read);
        this->_thread = std :: thread(&pool :: run, this);
    }

    // Destructor

    pool :: ~pool()
    {
        this->_alive = false;
        this->_wakepipe.wake();
        this->_thread.join();
    }

    // Methods

    pool :: connection pool :: bind(const :: drop :: connection & connection)
    {
        return (class connection){(*this), connection};
    }

    // Private methods

    promise <void> pool :: send(const std :: shared_ptr <:: drop :: connection :: arc> & arc, const buffer & message)
    {
        try
        {
            arc->send_lock();
            arc->block(false);
            arc->send_init(message);
        }
        catch(...)
        {
            return promise <void> :: rejected(std :: current_exception());
        }

        bool complete;

        try
        {
            complete = arc->send_step();
        }
        catch(...)
        {
            return promise <void> :: rejected(std :: current_exception());
        }

        if(complete)
        {
            try
            {
                arc->block(true);
                arc->send_unlock();
                return promise <void> :: resolved();
            }
            catch(...)
            {
                return promise <void> :: rejected(std :: current_exception());
            }
        }
        else
        {
            request request{.arc = arc, .type = queue :: write};

            this->_newmtx.lock();
            this->_new.push(request);
            this->_newmtx.unlock();

            this->_wakepipe.wake();

            promise <void> promise;

            request.promise.then([=]()
            {
                try
                {
                    arc->block(true);
                    arc->send_unlock();
                    promise.resolve();
                }
                catch(...)
                {
                    promise.reject(std :: current_exception());
                }
            }).except([=](const std :: exception_ptr & exception)
            {
                try
                {
                    arc->block(true);
                    arc->send_unlock();
                }
                catch(...)
                {
                }

                promise.reject(exception);
            });

            return promise;
        }
    }

    promise <buffer> pool :: receive(const std :: shared_ptr <:: drop :: connection :: arc> & arc)
    {
        try
        {
            arc->receive_lock();
            arc->block(false);
            arc->receive_init();
        }
        catch(...)
        {
            return promise <buffer> :: rejected(std :: current_exception());
        }

        bool complete;

        /*try
        {
            complete = arc->receive_step();
        }
        catch(...)
        {
            return promise <buffer> :: rejected(std :: current_exception());
        }*/ // REMOVE ME (comment): this disables the on-the-go receiving of data.

        complete = false;

        if(complete)
        {
            try
            {
                promise <buffer> promise = :: drop :: promise <buffer> :: resolved(arc->receive_yield());
                arc->block(true);
                arc->receive_unlock();
                return promise;
            }
            catch(...)
            {
                return promise <buffer> :: rejected(std :: current_exception());
            }
        }
        else
        {
            request request{.arc = arc, .type = queue :: read};

            this->_newmtx.lock();
            this->_new.push(request);
            this->_newmtx.unlock();

            this->_wakepipe.wake();

            promise <buffer> promise;

            request.promise.then([=]()
            {
                try
                {
                    buffer message = arc->receive_yield();
                    arc->block(true);
                    arc->receive_unlock();
                    promise.resolve(message);
                }
                catch(...)
                {
                    promise.reject(std :: current_exception());
                }
            }).except([=](const std :: exception_ptr & exception)
            {
                try
                {
                    arc->block(true);
                    arc->receive_unlock();
                }
                catch(...)
                {
                }

                promise.reject(exception);
            });

            return promise;
        }
    }

    void pool :: run()
    {
        while(true)
        {
            size_t count;

            try
            {
                count = this->_queue.select(settings :: interval);
            }
            catch(...)
            {
                continue;
            }

            if(!(this->_alive))
                break;

            for(size_t i = 0; i < count; i++)
            {
                if(this->_queue[i].descriptor() == this->_wakepipe)
                {
                    this->_wakepipe.flush();
                    continue;
                }

                auto & table = ((this->_queue[i].type() == queue :: write) ? this->_write : this->_read);
                request & request = table[this->_queue[i].descriptor()];

                if(this->_queue[i].error())
                    request.promise.reject(exceptions :: event_error());
                else
                {
                    try
                    {
                        if(!(request.type == queue :: write ? request.arc->send_step() : request.arc->receive_step()))
                            continue;

                        try
                        {
                            request.promise.resolve();
                        }
                        catch(const std :: exception & exception)
                        {
                        }
                    }
                    catch(...)
                    {
                        request.promise.reject(std :: current_exception());
                    }
                }

                try
                {
                    this->_queue.remove(this->_queue[i].descriptor(), this->_queue[i].type());
                }
                catch(...)
                {
                }

                table.erase(this->_queue[i].descriptor());
            }

            timestamp threshold = now;
            while(this->_timeouts.size() && this->_timeouts.front().timeout < threshold)
            {
                timeout & timeout = this->_timeouts.front();
                auto & table = (timeout.type == queue :: write ? this->_write : this->_read);

                try
                {
                    request & request = table.at(timeout.descriptor);

                    if(request.version == timeout.version)
                    {
                        if(timeout.type == queue :: write)
                            request.promise.reject(sockets :: exceptions :: send_timeout());
                        else
                            request.promise.reject(sockets :: exceptions :: receive_timeout());

                        try
                        {
                            this->_queue.remove(timeout.descriptor, timeout.type);
                        }
                        catch(...)
                        {
                        }

                        table.erase(timeout.descriptor);
                    }
                }
                catch(...)
                {
                }

                this->_timeouts.pop_front();
            }

            while(optional <request> request = this->_new.pop())
            {
                request->version = this->_version++;

                try
                {
                    this->_queue.add(request->arc->descriptor(), request->type);

                    (request->type == queue :: write ? this->_write : this->_read)[request->arc->descriptor()] = (*request);
                    this->_timeouts.push_back({.descriptor = request->arc->descriptor(), .type = request->type, .timeout = timestamp(now) + settings :: timeout, .version = request->version});
                }
                catch(...)
                {
                    request->promise.reject(std :: current_exception());
                }
            }
        }
    }
};
