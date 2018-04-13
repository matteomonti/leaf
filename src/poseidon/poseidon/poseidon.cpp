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

                if(entry.timestamp - timestamp(now) > settings :: intervals :: vote)
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
};
