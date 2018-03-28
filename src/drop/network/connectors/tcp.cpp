// Includes

#include "tcp.h"

namespace drop :: connectors
{
    // Exceptions

    const char * tcp :: exceptions :: event_error :: what() const throw()
    {
        return "Event error.";
    }

    // sync

    // Static methods

    connection tcp :: sync :: connect(const address & remote)
    {
        sockets :: tcp socket;
        socket.connect(remote);
        return connection(socket);
    }

    // async

    // Settings

    constexpr interval tcp :: async :: settings :: timeout;
    constexpr interval tcp :: async :: settings :: interval;

    // Constructors

    tcp :: async :: async() : _alive(true)
    {
        this->_queue.add(this->_wakepipe, queue :: read);
        this->_thread = std :: thread(&async :: run, this);
    }

    // Destructor

    tcp :: async :: ~async()
    {
        this->_alive = false;
        this->_wakepipe.wake();
        this->_thread.join();
    }

    // Methods

    promise <connection> tcp :: async :: connect(const address & remote)
    {
        request request;
        request.socket.block(false);

        this->_mutex.lock();

        try
        {
            request.socket.connect(remote);
            this->_new.push(request);
            this->_wakepipe.wake();
        }
        catch(...)
        {
            request.promise.reject(std :: current_exception());
        }

        this->_mutex.unlock();

        return request.promise;
    }

    // Private methods

    void tcp :: async :: run()
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
                if(this->_queue[i].type() == queue :: read)
                    this->_wakepipe.flush();
                else
                {
                    request request = this->_pending[this->_queue[i].descriptor()];
                    this->_pending.erase(this->_queue[i].descriptor());

                    try
                    {
                        this->_queue.remove(this->_queue[i].descriptor(), queue :: write);
                    }
                    catch(...)
                    {
                    }

                    if(this->_queue[i].error())
                        request.promise.reject(exceptions :: event_error());
                    else
                    {
                        try
                        {
                            request.socket.rethrow();
                            request.promise.resolve(connection(request.socket));
                        }
                        catch(...)
                        {
                            request.promise.reject(std :: current_exception());
                        }
                    }
                }
            }

            timestamp target = now;
            while(this->_timeouts.size() && (this->_timeouts.front().timeout < target))
            {
                try
                {
                    request request = this->_pending.at(this->_timeouts.front().descriptor);

                    if(request.version == this->_timeouts.front().version)
                    {
                        this->_pending.erase(this->_timeouts.front().descriptor);

                        try
                        {
                            this->_queue.remove(this->_timeouts.front().descriptor, queue :: write);
                        }
                        catch(...)
                        {
                        }

                        request.socket.close();
                        request.promise.reject(sockets :: exceptions :: connect_timeout());
                    }
                }
                catch(...)
                {
                }

                this->_timeouts.pop_front();
            }

            while(optional <request> request = this->_new.pop())
            {
                try
                {
                    this->_queue.add(request->socket.descriptor(), queue :: write);

                    request->version = this->_version++;
                    this->_pending[request->socket.descriptor()] = *request;
                    this->_timeouts.push_back({.descriptor = request->socket.descriptor(), .timeout = timestamp(now) + settings :: timeout, .version = request->version});
                }
                catch(...)
                {
                    request->promise.reject(std :: current_exception);
                }
            }
        }
    }
};
