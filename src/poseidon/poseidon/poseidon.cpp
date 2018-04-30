// Includes

#include "poseidon.h"

namespace poseidon
{
    // Constructors

    poseidon :: poseidon(const signer & signer, const view & view, dialer & dialer, pool & pool, crontab & crontab, const size_t & instanceid, std :: ostream & log) : _signer(signer), _brahms(signer, view, dialer, pool, crontab), _gossiper(signer, this->_brahms, dialer, pool, crontab), _publisher(crontab), _subscribers{}, _dialer(dialer), _pool(pool), _crontab(crontab), instanceid(instanceid), log(log)
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
            this->log << instanceid << " " << (uint64_t) timestamp(now) << " gossip " << statement.identifier() << " " << statement.sequence() << " " << statement.value() << std :: endl;

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
                {
                    if(this->_subscribers[index])
                        this->_subscribers[index]->once(statement.index());
                }
            }

            this->_mutex.unlock();
        });

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
            // log << "[slot " << slot << "] " << "Starting loop" << std :: endl;
            promise <void> disconnected;

            this->_mutex.lock();
            this->_sample[slot].clear();
            this->_mutex.unlock();

            try
            {
                // log << "[slot " << slot << "] " << "Establishing connection" << std :: endl;
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <2> (this->_brahms[slot]));

                // log << "[slot " << slot << "] " << "Connection established, creating subscriber" << std :: endl;
                subscriber <index, value> subscriber(connection, this->_crontab);

                subscriber.on <value> ([=](const index & index, const value & value)
                {
                    try
                    {
                        statement(index, value).verify();
                        // this->log << "Slot " << slot << " votes for " << index.identifier() << " / " << index.sequence() << ": " << value.value << std :: endl;
                        this->_mutex.lock();
                        this->_sample[slot][index] = value;
                        this->quorum(index);
                        this->_mutex.unlock();
                    }
                    catch(...) {}
                });

                subscriber.on <drop :: close> ([=]()
                {
                    disconnected.resolve();
                });

                this->_mutex.lock();

                for(const index & index : this->_pending)
                    subscriber.once(index);

                this->_subscribers[slot] = &subscriber;
                this->_mutex.unlock();

                subscriber.start();
                co_await first(this->_crontab.wait(settings :: brahms :: interval), disconnected);
                co_await this->_crontab.wait(10_ms); // THIS IS HERE ONLY TO PREVENT THE MESSENGER CALLBACK FROM DELETING ITSELF.
                // log << "[slot " << slot << "] " << "Disconnected or expired. Removing subscriber pointer" << std :: endl;

                this->_mutex.lock();
                this->_subscribers[slot] = nullptr;
                this->_mutex.unlock();
            }
            catch(...)
            {
                disconnected.resolve();
            }

            // log << "[slot " << slot << "] " << "Waiting before retrying" << std :: endl;
            co_await this->_crontab.wait(0.1_s);
        }
    }

    void poseidon :: quorum(const index & index)
    {
        if(!(this->_pending.count(index)))
            return;

        // log << "*************************" << std :: endl;
        // log << "Checking the quorum of " << index.identifier() << " / " << index.sequence() << std :: endl;

        std :: unordered_map <value, size_t, shorthash> votes;
        size_t total = 0;

        for(size_t slot = 0; slot < settings :: sample :: size; slot++)
            try
            {
                value value = this->_sample[slot].at(index);
                try
                {
                    votes.at(value)++;
                }
                catch(...)
                {
                    votes[value] = 1;
                }

                total++;
            }
            catch(...) {}

        // for(const auto & vote : votes)
            // log << vote.first.value << ": " << vote.second << std :: endl;

        /*if(total < settings :: poseidon :: thresholds :: quorum)
        {
            // log << "Not enough votes" << std :: endl;
            return;
        }*/

        struct
        {
            value value;
            size_t score;
        } best {.score = 0};

        for(const auto & vote : votes)
            if(vote.second > best.score)
            {
                best.value = vote.first;
                best.score = vote.second;
            }

        // log << "Best score: " << best.value.value << " with " << best.score << " votes" << std :: endl;
        log << instanceid << " " << (uint64_t) timestamp(now) << " vote " << index.identifier() << " " << index.sequence() << " " << best.value.value << " " << best.score << std :: endl;

        if(best.score >= settings :: poseidon :: thresholds :: accept)
        {
            // log << "Accepting the best value" << std :: endl;
            this->_votes[index] = {.value = best.value, .accepted = true};
            this->_pending.erase(index);
        }
        else if((settings :: poseidon :: thresholds :: accept - best.score) > (settings :: sample :: size - total))
        {
            // log << "There is no clear winner: dropping the statement" << std :: endl;
            this->_pending.erase(index);
        }

        // log << "*************************" << std :: endl;
    }
};
