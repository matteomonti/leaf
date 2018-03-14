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
        int wake[2];
        pipe(wake);

        this->_wake.read = wake[0];
        this->_wake.write = wake[1];

        fcntl(this->_wake.read, F_SETFL, O_NONBLOCK);
        this->_queue.add <queue :: read> (this->_wake.read);

        this->_thread = std :: thread(&pool :: run, this);
    }

    // Destructor

    pool :: ~pool()
    {
        this->_alive = false;
        this->wake();
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

        if(arc->send_step())
        {
            arc->send_unlock();

            promise <void> promise;
            promise.resolve();
            return promise;
        }
        else
        {
            return promise <void> (); // TODO: Make request and push it through the channel.
        }
    }

    void pool :: run()
    {
        while(true)
        {
            size_t count = this->_queue.select(settings :: interval);

            if(!(this->_alive))
                break;
        }
    }

    void pool :: wake()
    {
        char buffer = '\0';
        write(this->_wake.write, &buffer, 1);
    }
};
