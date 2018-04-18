// Includes

#include "poseidon.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // poseidon

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _gossiper(this->_signer.publickey(), (*this), crontab, log), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _sequence(0), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
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

        log << "------------> " << timestamp(now) << "Received statement from gossip: " << statement.identifier() << " / " << statement.sequence() << ": " << statement.value() << std :: endl;

        try
        {
            this->_logs.at(statement.index());
            log << "Statement collision!" << std :: endl;
        }
        catch(...)
        {
            log << "Adding statement to logs and checklist" << std :: endl;
            this->_logs[statement.index()] = entry{.value = statement.value(), .signature = statement.signature(), .timestamp = now, .accepted = false};
            this->_checklist.insert(statement.index());
        }

        this->_mutex.unlock();
    }

    promise <void> poseidon :: serve(pool :: connection connection)
    {
        log << "Serving connection" << std :: endl;
        std :: vector <index> queries = co_await connection.receive <std :: vector <index>> ();

        std :: vector <optional <value>> responses;
        responses.reserve(queries.size());

        this->_mutex.lock();

        if(queries.size() > 0)
            log << "Queries are: " << std :: endl;
        else
            log << "(no queries)" << std :: endl;

        for(size_t i = 0; i < queries.size(); i++)
            try
            {
                for(size_t i = 0; i < queries.size(); i++)
                    log << i << ": " << queries[i].identifier() << " / " << queries[i].sequence();

                entry entry = this->_logs.at(queries[i]);

                log << " (entry found)" << std :: endl;
                responses.push_back(value{.value = entry.value, .signature = entry.signature});
            }
            catch(...)
            {
                responses.push_back(null);
            }

        this->_mutex.unlock();

        log << "Sending responses" << std :: endl;
        co_await connection.send(responses);
    }

    promise <void> poseidon :: check(size_t version, size_t slot, optional <identifier> identifier, std :: vector <index> indexes)
    {
        if(identifier)
            log << "Checking with version " << version << ", slot " << slot << ", identifier " << *identifier << std :: endl;
        else
            log << "Checking with version " << version << ", slot " << slot << ", (no identifier)" << std :: endl;

        log << "Should check " << indexes.size() << " indexes" << std :: endl;

        try
        {
            if(identifier)
            {
                log << "Connecting" << std :: endl;
                pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <settings :: channel> (*identifier));

                log << "Sending indexes" << std :: endl;
                co_await connection.send(indexes);

                log << "Retrieving response values" << std :: endl;
                std :: vector <optional <value>> responses = co_await connection.receive <std :: vector <optional <value>>> ();

                if(responses.size() != indexes.size())
                    throw "Unexpected size"; // TODO: Add proper exception

                log << "Looping over responses" << std :: endl;

                for(size_t i = 0; i < responses.size(); i++)
                {
                    try
                    {
                        if(responses[i])
                        {
                            log << i << ": " << indexes[i].identifier() << " / " << indexes[i].sequence() << ": " << responses[i]->value << std :: endl;
                            statement statement(indexes[i], responses[i]->value, responses[i]->signature); // TODO: Make even more compact constructor with (index, value)

                            log << "Verifying statement" << std :: endl;
                            statement.verify();
                        }
                    }
                    catch(...)
                    {
                        log << "Exception here" << std :: endl;
                        responses[i] = null;
                    }
                }

                this->_mutex.lock();
                log << "Setting to checkpool" << std :: endl;
                this->_checkpool.set(version, slot, responses);
                this->_mutex.unlock();
            }
        }
        catch(const char * exception)
        {
            log << "There was an exception: " << exception << std :: endl;
        }
        catch(const std :: exception & exception)
        {
            log << "There was an exception: " << exception.what() << std :: endl;
        }
    }

    promise <void> poseidon :: run()
    {
        log << "Poseidon running.." << std :: endl;
        co_await this->_crontab.wait(settings :: intervals :: check);

        while(true)
        {
            log << "Going for check" << std :: endl;

            this->_mutex.lock();

            log << "Initializing checkpool" << std :: endl;
            size_t version = this->_checkpool.init(this->_checklist);

            std :: vector <index> indexes;
            indexes.reserve(this->_checklist.size());

            if(this->_checklist.size() > 0)
                log << "Checking indexes: " << std :: endl;
            else
                log << "(no indexes to check)" << std :: endl;

            for(const index & index : this->_checklist)
            {
                log << index.identifier() << " / " << index.sequence() << std :: endl;
                indexes.push_back(index);
            }

            this->_mutex.unlock();

            log << "Spawning checks: " << std :: endl;

            std :: array <optional <identifier>, brahms :: settings :: sample :: size> sample = this->_crawler.sample();

            for(size_t slot = 0; slot < brahms :: settings :: sample :: size; slot++)
            {
                if(sample[slot])
                    log << slot << ": " << *(sample[slot]) << std :: endl;
                else
                    log << slot << ": (no sample)" << std :: endl;

                this->check(version, slot, sample[slot], indexes);
            }

            co_await this->_crontab.wait(settings :: intervals :: check);

            log << "Time's up, evaluating checkpool" << std :: endl;

            this->_mutex.lock();

            this->_checkpool.evaluate <settings :: accept :: threshold> ([&](const statement & accept)
            {
                log << "------------> " << timestamp(now) << "Accepting " << accept.identifier() << " / " << accept.sequence() << ": " << accept.value() << std :: endl;
                this->_logs[accept.index()] = entry{.value = accept.value(), .signature = accept.signature(), .timestamp = 0, .accepted = true};
                this->_checklist.erase(accept.index());
            }, [&](const index & reject)
            {
                log << "Rejecting " << reject.identifier() << " / " << reject.sequence() << std :: endl;
                this->_checklist.erase(reject);
            });

            this->_mutex.unlock();
        }
    }
};
