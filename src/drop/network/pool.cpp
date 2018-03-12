// Includes

#include "pool.hpp"

namespace drop
{
    // Settings

    constexpr interval pool :: settings :: timeout;
    constexpr interval pool :: settings :: interval;

    // arc

    // Constructors

    pool :: connection :: arc :: arc(pool & pool, const :: drop :: connection & connection) : _pool(pool), _connection(connection._arc)
    {
    }

    // Destructor

    pool :: connection :: arc :: ~arc()
    {
    }

    // connection

    // Private constructors

    pool :: connection :: connection()
    {
    }

    pool :: connection :: connection(pool & pool, const :: drop :: connection & connection) : _arc(new arc(pool, connection))
    {
    }

    // Methods

    promise <void> pool :: connection :: send(const buffer & message) const
    {
        return this->_arc->_pool.send(*this, message);
    }

    promise <buffer> pool :: connection :: receive() const
    {
        return this->_arc->_pool.receive(*this);
    }

    // pool

    // Constructors

    pool :: pool() : _alive(true)
    {
        int wake[2];
        pipe(wake);

        this->_wake.read = wake[0];
        this->_wake.write = wake[1];

        fcntl(this->_wake.read, F_SETFL, O_NONBLOCK);
        this->_queue.add <queue :: read> (this->_wake.read);

        this->_thread = std :: thread(&pool :: run, this);
    }

    // Destructors

    pool :: ~pool()
    {
        this->_alive = false;
        this->wake();
        this->_thread.join();

        close(this->_wake.read);
        close(this->_wake.write);
    }

    // Methods

    pool :: connection pool :: bind(const :: drop :: connection & connection)
    {
        return {*this, connection};
    }

    // Private methods

    promise <void> pool :: send(const connection & connection, const buffer & message)
    {
        request request{.connection = connection, .type = queue :: write};

        connection._arc->_connection->send_init(message);

        if(!(connection._arc->_connection->send_step()))
        {
            this->_mutex.lock();

            this->_new.push(request);
            this->wake();

            this->_mutex.unlock();
        }
        else
            request.promise.resolve();

        return request.promise;
    }

    promise <buffer> pool :: receive(const connection & connection)
    {
        promise <buffer> promise;

        request request {.connection = connection, .type = queue :: read};
        connection._arc->_connection->receive_init();

        request.promise.then([=]()
        {
            try
            {
                promise.resolve(connection._arc->_connection->receive_yield());
            }
            catch(...)
            {
                promise.reject(std :: current_exception());
            }
        }).except([=](const std :: exception_ptr & exception)
        {
            promise.reject(exception);
        });

        this->_mutex.lock();

        this->_new.push(request);
        this->wake();

        this->_mutex.unlock();
        return promise;
    }

    void pool :: run()
    {
        while(true)
        {
            size_t count = this->_queue.select(settings :: interval);

            if(!(this->_alive))
                break;

            for(size_t i = 0; i < count; i++)
            {
                if(this->_queue[i].descriptor() == this->_wake.read)
                {
                    char buffer[1];
                    while(read(this->_wake.read, buffer, 1) >= 0);

                    continue;
                }

                request request = this->_pending[this->_queue[i].descriptor()];

                try
                {
                    queue :: type type = this->_queue[i].type();

                    if(!((type == queue :: read) ? request.connection._arc->_connection->receive_step() : request.connection._arc->_connection->send_step()))
                        continue;

                    request.promise.resolve();
                }
                catch(...)
                {
                    request.promise.reject(std :: current_exception());
                }

                this->_pending.erase(this->_queue[i].descriptor());

                if(this->_queue[i].type() == queue :: read)
                    this->_queue.remove <queue :: read> (this->_queue[i].descriptor());
                else
                    this->_queue.remove <queue :: write> (this->_queue[i].descriptor());
            }

            timestamp target = now;
            while(this->_timeouts.size() && this->_timeouts.front().timeout < target)
            {
                try
                {
                    request request = this->_pending.at(this->_timeouts.front().descriptor);

                    if(request.version == this->_timeouts.front().version)
                    {
                        this->_pending.erase(this->_timeouts.front().descriptor);

                        if(request.type == queue :: read)
                        {
                            this->_queue.remove <queue :: read> (this->_timeouts.front().descriptor);
                            request.promise.reject(sockets :: exceptions :: receive_timeout());
                        }
                        else
                        {
                            this->_queue.remove <queue :: write> (this->_timeouts.front().descriptor);
                            request.promise.reject(sockets :: exceptions :: send_timeout());
                        }
                    }
                }
                catch(...)
                {
                }

                this->_timeouts.pop_front();
            }

            while(optional <request> request = this->_new.pop())
            {
                if(request->type == queue :: read)
                    this->_queue.add <queue :: read> (request->connection._arc->_connection->descriptor());
                else
                    this->_queue.add <queue :: write> (request->connection._arc->_connection->descriptor());

                request->version = this->_version++;

                this->_pending[request->connection._arc->_connection->descriptor()] = (*request);
                this->_timeouts.push_back({.descriptor = request->connection._arc->_connection->descriptor(), .timeout = timestamp(now) + settings :: timeout, .version = request->version});
            }
        }
    }

    void pool :: wake()
    {
        char buffer[] = {'\0'};
        write(this->_wake.write, buffer, 1);
    }
};
