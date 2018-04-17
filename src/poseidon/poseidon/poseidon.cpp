// Includes

#include "poseidon.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // poseidon

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab) : _signer(signer), _gossiper(this->_signer.publickey(), (*this), crontab), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _sequence(0), _dialer(dialer), _pool(pool), _crontab(crontab)
    {
    }

    // Methods

    void poseidon :: start()
    {
        this->_dialer.on <settings :: channel> ([=](const connection & connection)
        {
            this->serve(this->_pool.bind(connection));
        });

        this->_crawler.start();
        this->run();
    }

    void poseidon :: publish(const buffer & value)
    {
        this->_mutex.lock();

        class statement statement(this->_signer, this->_sequence, value);
        this->_gossiper.add(statement);
        this->_sequence++;

        this->_mutex.unlock();
    }

    // Private methods

    void poseidon :: gossip(const statement & statement)
    {
        this->_mutex.lock();

        try
        {
            this->_logs.at(statement.index());
        }
        catch(...)
        {
            this->_logs[statement.index()] = entry{.value = statement.value(), .signature = statement.signature(), .timestamp = now, .accepted = false};
            this->_checklist.insert(statement.index());
        }

        this->_mutex.unlock();
    }

    promise <void> poseidon :: serve(pool :: connection connection)
    {
        std :: vector <index> queries = co_await connection.receive <std :: vector <index>> ();

        std :: vector <optional <value>> responses;
        responses.reserve(queries.size());

        this->_mutex.lock();

        for(size_t i = 0; i < queries.size(); i++)
            try
            {
                entry entry = this->_logs.at(queries[i]);

                if(timestamp(now) - entry.timestamp > settings :: intervals :: vote)
                    responses.push_back(value{.value = entry.value, .signature = entry.signature});
                else
                    responses.push_back(null);
            }
            catch(...)
            {
                responses.push_back(null);
            }

        this->_mutex.unlock();

        co_await connection.send(responses);
    }

    promise <void> poseidon :: check(size_t version, size_t slot, optional <identifier> identifier, std :: vector <index> indexes)
    {
        try
        {
            if(identifier)
            {
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (*identifier));
                co_await connection.send(indexes);

                std :: vector <optional <value>> responses = co_await connection.receive <std :: vector <optional <value>>> ();

                if(responses.size() != indexes.size())
                    throw "Unexpected size"; // TODO: Add proper exception

                for(size_t i = 0; i < responses.size(); i++)
                {
                    try
                    {
                        if(responses[i])
                        {
                            statement statement(indexes[i], responses[i]->value, responses[i]->signature); // TODO: Make even more compact constructor with (index, value)
                            statement.verify();
                        }
                    }
                    catch(...)
                    {
                        responses[i] = null;
                    }
                }

                this->_mutex.lock();

                this->_checkpool.set(version, slot, responses);
                this->_mutex.unlock();
            }
        }
        catch(...)
        {
        }
    }

    promise <void> poseidon :: run()
    {
        co_await this->_crontab.wait(settings :: intervals :: check);

        while(true)
        {
            this->_mutex.lock();

            size_t version = this->_checkpool.init(this->_checklist);

            std :: vector <index> indexes;
            indexes.reserve(this->_checklist.size());

            for(const index & index : this->_checklist)
                indexes.push_back(index);

            this->_mutex.unlock();

            std :: array <optional <identifier>, brahms :: settings :: sample :: size> sample = this->_crawler.sample();

            for(size_t slot = 0; slot < brahms :: settings :: sample :: size; slot++)
                this->check(version, slot, sample[slot], indexes);

            co_await this->_crontab.wait(settings :: intervals :: check);

            this->_mutex.lock();

            this->_checkpool.evaluate <settings :: accept :: threshold> ([&](const statement & accept)
            {
                this->emit <statement> (accept);
                this->_logs[accept.index()] = entry{.value = accept.value(), .signature = accept.signature(), .timestamp = 0, .accepted = true};
                this->_checklist.erase(accept.index());
            }, [&](const index & reject)
            {
                this->_checklist.erase(reject);
            });

            this->_mutex.unlock();
        }
    }
};
