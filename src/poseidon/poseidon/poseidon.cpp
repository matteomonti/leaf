// Includes

#include "poseidon.h"

namespace poseidon
{
    // Constructors

    poseidon :: poseidon(const signer & signer, const view & view, dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _brahms(signer, view, dialer, pool, crontab), _gossiper(signer, this->_brahms, dialer, pool, crontab), _publisher(crontab), _subscribers{}, _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
    }

    // Methods

    void poseidon :: start()
    {
        this->_dialer.on <2> ([=](const dial & dial)
        {
            this->_publisher.serve(this->_pool.bind(dial));
        });

        /*this->_publisher.on <publisher <index, value> :: archive> ([=](const index & index, const publisher <class index, value> :: archive & archive)
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
            this->log << "Gossiped statement: " << statement.identifier() << " / " << statement.sequence() << ": " << statement.value() << std :: endl;
            this->_mutex.lock();

            try
            {
                this->_votes.at(statement.index());
            }
            catch(...)
            {
                this->log << "Voting for it" << std :: endl;
                this->_votes[statement.index()] = {.value = {statement.value(), statement.signature()}, .accepted = false};

                this->log << "Publishing it" << std :: endl;
                this->_publisher.publish(statement.index(), {statement.value(), statement.signature()});

                this->log << "Inserting it into pending" << std :: endl;
                this->_pending.insert(statement.index());

                this->log << "Subscribing to it" << std :: endl;
                for(size_t index = 0; index < settings :: sample :: size; index++)
                {
                    if(this->_subscribers[index])
                    {
                        this->log << "Subscriber " << index << " is non-null" << std :: endl;
                        this->_subscribers[index]->once(statement.index());
                        this->log << "Subscribed" << std :: endl;
                    }
                }

                this->log << "All subscribed" << std :: endl;
            }

            this->_mutex.unlock();
        });*/

        this->_brahms.start();
        this->_gossiper.start();

        for(size_t index = 0; index < settings :: sample :: size; index++)
            this->subscribe(index);
    }

    void poseidon :: publish(const size_t & sequence, const buffer & value)
    {
        this->_gossiper.publish({this->_signer, sequence, value});
    }

    // Private methods

    promise <void> poseidon :: subscribe(size_t slot)
    {
        while(true)
        {
            log << "[slot " << slot << "] " << "Starting loop" << std :: endl;
            promise <void> disconnected;

            this->_mutex.lock();
            this->_sample[slot].clear();
            this->_mutex.unlock();

            try
            {
                log << "[slot " << slot << "] " << "Establishing connection" << std :: endl;
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <2> (this->_brahms[slot]));

                log << "[slot " << slot << "] " << "Connection established, creating subscriber" << std :: endl;
                subscriber <index, value> subscriber(connection, this->_crontab);

                log << "[slot " << slot << "] " << "Subscriber created" << std :: endl;

                subscriber.on <value> ([=](const index & index, const value & value)
                {
                    this->log << "Slot " << slot << " votes for " << index.identifier() << " / " << index.sequence() << ": " << value.value << std :: endl;
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

                log << "[slot " << slot << "] " << "Subscribing to pending indexes" << std :: endl;

                for(const index & index : this->_pending)
                {
                    log << "[slot " << slot << "] " << "Subscribing to " << index.identifier() << " / " << index.sequence() << std :: endl;
                    subscriber.once(index);
                }

                log << "[slot " << slot << "] " << "Setting subscriber pointer" << std :: endl;
                this->_subscribers[slot] = &subscriber;
                this->_mutex.unlock();

                log << "[slot " << slot << "] " << "Starting subscriber" << std :: endl;
                subscriber.start();
                log << "[slot " << slot << "] " << "Subscriber started" << std :: endl;
            }
            catch(const std :: exception & exception)
            {
                log << "[slot " << slot << "] " << "Exception: " << exception.what() << std :: endl;
                disconnected.resolve();
            }
            catch(...)
            {
                log << "[slot " << slot << "] " << "Unknown exception" << std :: endl;
                disconnected.resolve();
            }

            log << "[slot " << slot << "] " << "Awaiting for interval or disconnection" << std :: endl;
            co_await first(this->_crontab.wait(settings :: brahms :: interval), disconnected);

            log << "[slot " << slot << "] " << "Disconnected or expired. Removing subscriber pointer" << std :: endl;

            this->_mutex.lock();
            this->_subscribers[slot] = nullptr;
            this->_mutex.unlock();

            log << "[slot " << slot << "] " << "Waiting before retrying" << std :: endl;
            co_await this->_crontab.wait(5_s);
        }
    }

    void poseidon :: quorum(const index & index)
    {
        // TODO: Check the quorum!
    }
};
