// Includes

#include "pool.hpp"

namespace drop
{
    // connection

    // Private constructors

    pool :: connection :: connection(pool & pool, const :: drop :: connection & connection) : _pool(pool), _arc(connection._arc)
    {
    }

    // Methods

    promise <void> pool :: connection :: send(const buffer & message) const
    {
        return this->_pool.send(this->_arc, message);
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
        arc->send_lock();
        arc->send_init(message);

        if(false && arc->send_step()) // REMOVE ME
        {
            arc->send_unlock();

            promise <void> promise;
            promise.resolve();
            return promise;
        }
        else
        {
            request request{.arc = arc, .type = queue :: write};
            this->_new.push(request);
            this->_wakepipe.wake();
            return request.promise;
        }
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
                std :: cout << "Processing event " << i << std :: endl;

                if(this->_queue[i].descriptor() == this->_wakepipe)
                {
                    std :: cout << "(Wakepipe)" << std :: endl;
                    this->_wakepipe.flush();
                    continue;
                }

                auto & table = ((this->_queue[i].type() == queue :: write) ? this->_write : this->_read);
                request request = table[this->_queue[i].descriptor()];

                try
                {
                    if(!(request.type == queue :: write ? request.arc->send_step() : request.arc->receive_step()))
                        continue;

                    request.promise.resolve();
                }
                catch(...)
                {
                    request.promise.reject(std :: current_exception());
                }

                table.erase(this->_queue[i].descriptor());
                this->_queue.remove(this->_queue[i].descriptor(), this->_queue[i].type());
            }

            // TODO: Manage timeouts

            while(optional <request> request = this->_new.pop())
            {
                request->version = this->_version++;

                (request->type == queue :: write ? this->_write : this->_read)[request->arc->descriptor()] = (*request);
                this->_timeouts.push_back({.descriptor = request->arc->descriptor(), .type = request->type, .timeout = timestamp(now) + settings :: timeout, .version = request->version});

                this->_queue.add(request->arc->descriptor(), request->type);
            }
        }
    }
};
