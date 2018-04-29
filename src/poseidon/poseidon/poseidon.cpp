// Includes

#include "poseidon.h"

namespace poseidon
{
    // Constructors

    poseidon :: poseidon(const signer & signer, const view & view, dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _brahms(signer, view, dialer, pool, crontab), _gossiper(signer, this->_brahms, dialer, pool, crontab), _publisher(crontab), _subscribers{}, _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }

    // Methods

    void poseidon :: start()
    {
        this->_dialer.on <2> ([=](const dial & dial)
        {
            this->_publisher.serve(this->_pool.bind(dial));
        });

        this->_publisher.on <publisher <index, value> :: archive> ([=](const index & index, const publisher <class index, value> :: archive & archive)
        {
            this->_mutex.lock();

            try
            {
                archive << this->_votes.at(index).value;
            }
            catch(...)
            {
            }

            this->_mutex.unlock();
        });

        this->_gossiper.on <statement> ([=](const statement & statement)
        {
            this->_mutex.lock();

            try
            {
                this->_votes.at(statement.index());
            }
            catch(...)
            {
                this->_votes[statement.index()] = {.value = {statement.value(), statement.signature()}, .accepted = false};
                this->_publisher.publish(statement.index(), {statement.value(), statement.signature()});

                this->_pending.insert(statement.index());
                for(size_t index = 0; index < settings :: sample :: size; index++)
                    if(this->_subscribers[index])
                        this->_subscribers[index]->subscribe(statement.index());
            }

            this->_mutex.unlock();
        });

        this->_brahms.start();
        this->_gossiper.start();

        for(size_t index = 0; index < settings :: sample :: size; index++)
            this->subscribe(index);
    }

    // Private methods

    promise <void> poseidon :: subscribe(size_t slot)
    {
        while(true)
        {
            promise <void> disconnected;

            this->_mutex.lock();
            this->_sample[slot].clear();
            this->_mutex.unlock();

            try
            {
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <2> (this->_brahms[slot]));
                subscriber <index, value> subscriber(connection, this->_crontab);

                subscriber.on <value> ([=](const index & index, const value & value)
                {
                    this->_mutex.lock();
                    this->_sample[slot][index] = value;
                    this->quorum(index);
                    this->_mutex.unlock();
                });

                subscriber.on <drop :: close> ([=]()
                {
                    disconnected.resolve();
                });

                this->_mutex.lock();

                for(const index & index : this->_pending)
                    subscriber.subscribe(index);

                this->_subscribers[slot] = &subscriber;
                this->_mutex.unlock();
            }
            catch(...)
            {
                continue;
            }

            co_await first(this->_crontab.wait(settings :: brahms :: interval), disconnected);

            this->_mutex.lock();
            this->_subscribers[slot] = nullptr;
            this->_mutex.unlock();
        }
    }

    void poseidon :: quorum(const index & index)
    {
        // TODO: Check the quorum!
    }
};
