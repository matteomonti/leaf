// Includes

#include "peer.h"

namespace poseidon :: staticsample
{
    using namespace drop;
    using namespace vine;

    // Constructors

    peer :: peer(const size_t & id, const signer & signer, const interval & rate, const std :: vector <identifier> & view, const std :: vector <identifier> & sample, const address & directory) : _id(id), _signer(signer), _rate(rate), _dialer(directory, this->_signer, this->_connector, this->_pool, this->_crontab), _publisher(this->_crontab), _gossiper(this->_crontab), _view(view), _sample(sample), _subscribers(new subscriber <uint64_t, buffer> * [this->_sample.size()] {})
    {
        this->_publisher.on <publisher <uint64_t, buffer> :: archive> ([=](const auto & id, const auto & archive)
        {
            this->_mutex.lock();

            try
            {
                buffer message = this->_statements.at(id);
                archive << message;
            }
            catch(...)
            {
            }

            this->_mutex.unlock();
        });

        this->_gossiper.on <statement> ([=](const auto & id, const statement & statement)
        {
            std :: cout << (uint64_t) timestamp(now) << " gossip " << statement.id << ": " << statement.message << std :: endl; // TODO: Maybe refactor with event emitter.

            this->_mutex.lock();
            this->_statements[statement.id] = statement.message;
            this->_votes[statement.id] = 0;

            for(size_t i = 0; i < this->_sample.size(); i++)
                if(this->_subscribers[i])
                    this->_subscribers[i]->once(statement.id);

            this->_mutex.unlock();

            this->_publisher.publish(statement.id, statement.message);
        });

        this->_dialer.on <dial> ([=](dial dial) -> promise <void>
        {
            try
            {
                auto connection = this->_pool.bind(dial);
                bool channel = co_await connection.receive <bool> ();

                if(!channel)
                    this->_gossiper.serve(connection, this->_signer.publickey() < dial.identifier());
                else
                    this->_publisher.serve(connection);
            }
            catch(...)
            {
            }
        });
    }

    // Methods

    void peer :: start()
    {
        if(this->_id == 0)
            this->seed();

        for(size_t index = 0; index < this->_view.size(); index++)
            this->maintain_gossip(index);

        for(size_t index = 0; index < this->_sample.size(); index++)
            this->connect_subscriber(index);
    }

    // Private methods

    promise <void> peer :: maintain_gossip(size_t index)
    {
        while(true)
        {
            try
            {
                dial dial = co_await this->_dialer.connect(this->_view[index]);
                dial.send(false);

                co_await this->_gossiper.serve(this->_pool.bind(dial), this->_signer.publickey() < dial.identifier());
            }
            catch(...)
            {
            }

            co_await this->_crontab.wait(0.5_s);
        }
    }

    promise <void> peer :: connect_subscriber(size_t index)
    {
        while(true)
        {
            try
            {
                dial dial = co_await this->_dialer.connect(this->_sample[index]);
                dial.send(true);

                if(this->_subscribers[index])
                    delete this->_subscribers[index];

                this->_mutex.lock();

                this->_subscribers[index] = new subscriber <uint64_t, buffer> (this->_pool.bind(dial), this->_crontab);
                this->_subscribers[index]->on <buffer> ([=](const uint64_t & id, const buffer & message)
                {
                    this->_mutex.lock();
                    this->_votes[id]++;
                    std :: cout << (uint64_t) timestamp(now) << " votes " << id << " " << this->_votes[id] << std :: endl;

                    if(this->_votes[id] == this->_sample.size())
                    {
                        std :: cout << (uint64_t) timestamp(now) << " accept " << id << ": " << message << std :: endl;
                        this->_votes.erase(id);
                    }

                    this->_mutex.unlock();
                });

                this->_subscribers[index]->on <drop :: close> ([=]()
                {
                    this->connect_subscriber(index);
                });

                for(const auto & vote : this->_votes)
                    this->_subscribers[index]->once(vote.first);

                this->_mutex.unlock();
                this->_subscribers[index]->start();

                co_return;
            }
            catch(...)
            {
            }

            co_await this->_crontab.wait(0.5_s);
        }
    }

    promise <void> peer :: seed()
    {
        for(uint64_t round = 0;; round++)
        {
            std :: cout << (uint64_t) timestamp(now) << " seed " << round << std :: endl;

            char message[1024];
            sprintf(message, "Message number %llu", round);
            this->_gossiper.publish({.id = round, .message = message});

            co_await this->_crontab.wait(this->_rate);
        }
    }
};
