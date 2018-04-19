// Includes

#include "poseidon.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    poseidon :: poseidon(const signer & signer, const std :: array <identifier, brahms :: settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _gossiper(this->_signer.publickey(), (*this), crontab, log), _crawler(this->_signer, view, this->_gossiper, dialer, pool, crontab), _checkpool(*this, settings :: accept :: threshold), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
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

    void poseidon :: publish(const uint64_t & sequence, const buffer & value)
    {
        this->_gossiper.publish(statement(this->_signer, sequence, value));
    }

    // Private methods

    void poseidon :: accept(const statement & statement)
    {
        this->_votes[statement.index()] = {.value = {statement.value(), statement.signature()}, .accepted = true};
        this->emit <events :: accept> (statement);
    }

    void poseidon :: gossip(const statement & statement)
    {
        this->emit <events :: gossip> (statement);

        this->_mutex.lock();

        try
        {
            this->_votes.at(statement.index());
            log << "Statement collision!" << std :: endl;
        }
        catch(...)
        {
            log << "Adding statement to votes and checkpool" << std :: endl;
            this->_votes[statement.index()] = vote{.value = {statement.value(), statement.signature()}, .accepted = false};
            this->_checkpool.add(statement.index());

            log << "Dispatching to all servers" << std :: endl;

            std :: vector <checker :: server *> garbage;
            for(checker :: server * server : this->_servers)
            {
                if(server->alive())
                    server->push(statement);
                else
                    garbage.push_back(server);
            }

            for(checker :: server * server : garbage)
            {
                delete server;
                this->_servers.erase(server);
            }
        }

        this->_mutex.unlock();
    }

    void poseidon :: serve(const pool :: connection & connection)
    {
        this->_mutex.lock();
        checker :: server * server = new checker :: server(connection, this->_votes, this->_mutex);
        this->_servers.insert(server);
        this->_mutex.unlock();
    }
};
