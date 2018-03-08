// Includes

#include "crontab.h"

namespace drop
{
    // event

    // Constructors

    crontab :: event :: event(const timestamp & time) : _time(time)
    {
    }

    // Getters

    const promise <void> & crontab :: event :: promise() const
    {
        return this->_promise;
    }

    const timestamp & crontab :: event :: time() const
    {
        return this->_time;
    }

    // Operators

    bool crontab :: event :: operator < (const event & rho) const
    {
        return this->_time > rho._time;
    }

    bool crontab :: event :: operator <= (const event & rho) const
    {
        return this->_time >= rho._time;
    }

    bool crontab :: event :: operator > (const event & rho) const
    {
        return this->_time < rho._time;
    }

    bool crontab :: event :: operator >= (const event & rho) const
    {
        return this->_time <= rho._time;
    }

    bool crontab :: event :: operator == (const event & rho) const
    {
        return this->_time == rho._time;
    }

    bool crontab :: event :: operator != (const event & rho) const
    {
        return this->_time != rho._time;
    }

    // bucket

    // Methods

    void crontab :: bucket :: lock()
    {
        this->_mutex.lock();
    }

    void crontab :: bucket :: unlock()
    {
        this->_mutex.unlock();
    }

    // crontab

    // Constructors

    crontab :: crontab() : _buckets(new bucket[settings :: bucket :: count]), _cursor(timestamp(now) / settings :: bucket :: size), _alive(true), _thread(&crontab :: run, this)
    {
    }

    // Destructor

    crontab :: ~crontab()
    {
        this->_alive = false;
        this->_semaphore.post();
        this->_thread.join();

        delete [] this->_buckets;
    }

    // Methods

    promise <void> crontab :: wait(const timestamp & timestamp)
    {
        event event(timestamp);

        size_t index = (event.time() / settings :: bucket :: size);
        bucket & bucket = this->_buckets[index % settings :: bucket :: count];

        bucket.lock();
        bucket.push(event);
        bucket.unlock();

        if(index <= this->_cursor)
            this->_semaphore.post();

        return event.promise();
    }

    promise <void> crontab :: wait(const interval & interval)
    {
        return this->wait(timestamp(now) + interval);
    }

    // Private methods

    void crontab :: run()
    {
        while(this->_alive)
        {
            timestamp target = now;
            timestamp next;

            this->_cursor--;
            while(true)
            {
                bucket & bucket = this->_buckets[this->_cursor % settings :: bucket :: count];

                bucket.lock();
                while(!(bucket.empty()) && bucket.top().time() < target)
                {
                    bucket.top().promise().resolve();
                    bucket.pop();
                }
                bucket.unlock();

                if(this->_cursor == (target / settings :: bucket :: size))
                {
                    if(!(bucket.empty()))
                        next = bucket.top().time();
                    else
                        next = timestamp((this->_cursor + 1) * settings :: bucket :: size);

                    break;
                }
                else
                    this->_cursor++;
            }

            if(next - target > settings :: min_wait)
                this->_semaphore.wait(next);
            else
                this->_semaphore.wait(next + settings :: min_wait);
        }
    }
};
